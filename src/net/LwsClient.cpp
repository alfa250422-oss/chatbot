#include "net/WsClient.hpp"
#include <libwebsockets.h>
#include <cstring>

static int cb(lws* wsi, lws_callback_reasons reason, void* user, void* in, size_t len){
  auto* self = reinterpret_cast<WsClient*>(lws_wsi_user(wsi));
  switch(reason){
    case LWS_CALLBACK_CLIENT_ESTABLISHED:
        if(self) self->emitOpen();
        break;
    case LWS_CALLBACK_CLIENT_RECEIVE:
      if(self){
          if(self)
              if(lws_frame_is_binary(wsi))
                  self->emitBinary((const uint8_t*)in,len);
              else{
                  self->emitText(std::string((const char*)in,len));
              }
          else{
              if(self->onText)
                  self->onText(std::string((const char*)in,len));
          }
      }
          case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: if(self) self->emitClose(-1); break;
          case LWS_CALLBACK_WS_PEER_INITIATED_CLOSE: if(self) self->emitClose(0); break;
      break;

    default: break;
  }
  return 0;
}

bool WsClient::connect(const std::string& url, const std::vector<std::pair<std::string,std::string>>& headers){
  lws_context_creation_info info{}; info.options=LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT; info.port=CONTEXT_PORT_NO_LISTEN; static lws_protocols prot[]={{"realtime", cb, 0, 64*1024},{nullptr,nullptr,0,0}}; info.protocols=prot; ctx=lws_create_context(&info); if(!ctx) return false;
  lws_client_connect_info i{}; i.context=ctx; i.userdata=this; i.protocol=prot[0].name;
  i.address = "api.openai.com"; i.host = i.address; i.path = "/v1/realtime?model=gpt-4o-realtime-preview-2024-12-17"; i.port=443; i.ssl_connection=LCCSCF_USE_SSL; i.origin=i.host; i.method="GET"; i.alpn="h2;http/1.1";
  std::string hdrs; for(auto& kv:headers){ hdrs += kv.first+": "+kv.second+"\r\n"; } i.headers = hdrs.c_str(); i.headers_len=(int)hdrs.size();
  wsi = lws_client_connect_via_info(&i); return wsi!=nullptr;
}

bool WsClient::sendText(const std::string& s){ if(!wsi) return false; std::vector<unsigned char> buf(LWS_PRE+s.size()); memcpy(buf.data()+LWS_PRE,s.data(),s.size()); return lws_write(wsi, buf.data()+LWS_PRE, s.size(), LWS_WRITE_TEXT) >= 0; }
bool WsClient::sendBin(const uint8_t* p, size_t n){ if(!wsi) return false; std::vector<unsigned char> buf(LWS_PRE+n); memcpy(buf.data()+LWS_PRE,p,n); return lws_write(wsi, buf.data()+LWS_PRE, n, LWS_WRITE_BINARY) >= 0; }
void WsClient::poll(int ms){ if(ctx) lws_service(ctx, ms);} void WsClient::close(){ if(ctx){ lws_context_destroy(ctx); ctx=nullptr; wsi=nullptr; }} WsClient::~WsClient(){ close(); }

void WsClient::emitOpen(){ if(onOpen) onOpen(); }
void WsClient::emitClose(int c){ if(onClose) onClose(c); }
void WsClient::emitText(const std::string& s){ if(onText) onText(s); }
void WsClient::emitBinary(const uint8_t* p, size_t n){ if(onBinary) onBinary(p,n); }