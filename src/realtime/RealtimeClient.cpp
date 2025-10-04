#include "realtime/RealtimeClient.hpp"
#include "utils/Json.hpp"
#include "utils/Base64.hpp"
#include <cjson/cJSON.h>
#include <cstdlib>
#include <cstdio>

bool RealtimeClient::connect(const RealtimeConfig& cfg){
  rc = cfg;
  std::vector<std::pair<std::string,std::string>> headers = {
    {"Authorization", std::string("Bearer ") + rc.api_key},
    {"OpenAI-Beta", "realtime=v1"}
  };
  ws.setOnOpen([this]{ ws.sendText(makeSessionUpdate()); });
  ws.setOnText([this](const std::string& s){ handleServerEvent(s); });
  return ws.connect("wss://api.openai.com/v1/realtime", headers);
}

std::string RealtimeClient::makeSessionUpdate(){
  using namespace json;
  auto root = obj(); add(root, "type", "session.update");
  auto session = obj();
  auto modalities = arr(); cJSON_AddItemToArray(modalities, cJSON_CreateString("text")); cJSON_AddItemToArray(modalities, cJSON_CreateString("audio"));
  add(session, "modalities", modalities);
  add(session, "instructions", rc.instructions.c_str());
  add(session, "input_audio_format", "pcm16"); // 24kHz, 16bit, mono
  add(session, "output_audio_format", "pcm16");
  // 自动转写 + 服务器 VAD，结束后自动创建响应
  auto asr = obj(); add(asr, "model", "whisper-1");
  add(session, "input_audio_transcription", asr);
  if(rc.server_vad){ auto vad=obj();
      add(vad,"type","server_vad");
      add(vad,"threshold",0.5); add(vad,"create_response",true);
      add(session,"turn_detection", vad);
  }
  add(root, "session", session);
  auto s = str(root); free(root); return s;
}

void RealtimeClient::sendAudioPCM16(const int16_t* pcm, size_t samples){
  using namespace json; auto root = obj(); add(root,"type","input_audio_buffer.append"); auto b = b64::encode_pcm16(pcm, samples); add(root,"audio", b.c_str()); auto s=str(root); free(root); ws.sendText(s);
}

void RealtimeClient::commitInput(){ using namespace json; auto r=obj(); add(r,"type","input_audio_buffer.commit"); auto s=str(r); free(r); ws.sendText(s);} 
void RealtimeClient::createResponse(){ using namespace json; auto r=obj(); add(r,"type","response.create"); auto s=str(r); free(r); ws.sendText(s);} 
void RealtimeClient::poll(int ms){ ws.poll(ms);} void RealtimeClient::close(){ ws.close(); }

static std::string json_get_str(cJSON* o, const char* k){ auto* it=cJSON_GetObjectItemCaseSensitive(o,k); return (it && cJSON_IsString(it)&&it->valuestring)?it->valuestring:""; }

void RealtimeClient::handleServerEvent(const std::string& jt){
  cJSON* root = cJSON_Parse(jt.c_str()); if(!root) return; std::string type = json_get_str(root, "type");
  if(type=="session.updated"){ std::puts("[realtime] session.updated"); }
  else if(type=="response.output_text.delta"){ auto* d=cJSON_GetObjectItem(root,"delta"); if(d&&cJSON_IsString(d)){ textBuf+=d->valuestring; if(onText) onText(d->valuestring);} }
  else if(type=="response.output_audio.delta"){ auto* d=cJSON_GetObjectItem(root,"delta"); if(d&&cJSON_IsString(d)){
      auto bytes = b64::decode(d->valuestring); // PCM16LE @24kHz mono
      // bytes.size() should be even
      if(onPcm && bytes.size()>=2){ onPcm(reinterpret_cast<int16_t*>(bytes.data()), bytes.size()/2); }
    }}
  else if(type=="response.done"){ std::printf("[realtime] response.done, text=%zu chars\n", textBuf.size()); textBuf.clear(); }
  else {
    // 其它事件按需处理，如 conversation.item.input_audio_transcription.completed
  }
  cJSON_Delete(root);
}