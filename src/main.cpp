#include <cstdio>
#include <cstdlib>
#include <string>
#include <atomic>
#include "audio/AudioIn.hpp"
#include "audio/AudioOut.hpp"
#include "audio/Vad.hpp"
#include "realtime/RealtimeClient.hpp"
#include "safety/Moderation.hpp"

int main(){
  const char* key = std::getenv("OPENAI_API_KEY");
  if(!key){ std::fprintf(stderr, "[ERROR] Missing env OPENAI_API_KEY\n"); return 1; }

  AudioIn ain; AudioOut aout; Vad vad; Moderation mod;
  aout.start(24000,1);

  RealtimeClient rtc; 
  rtc.setOnText([&](const std::string& s){ std::printf("[TEXT] %s", s.c_str()); });
  rtc.setOnPcm([&](const int16_t* pcm, size_t n){ aout.play(pcm, n); });

  if(!rtc.connect(RealtimeConfig{ .api_key = key })){
    std::fprintf(stderr, "[ERROR] realtime connect failed\n"); return 2; }

  // 采集并推送 20ms 帧（24kHz => 每帧480样本）
  ain.start(24000,1,20,[&](const int16_t* pcm, size_t N){
    // 简单 VAD：静音不推送，语音直接 append（server_vad 模式会自动 commit + create）
    if(vad.isSpeech(pcm, N)) rtc.sendAudioPCM16(pcm, N);
  });

  std::puts("[chatbot] Start speaking. Press Ctrl+C to exit.");
  while(true){ rtc.poll(10); }
  return 0;
}