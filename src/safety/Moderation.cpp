#include "safety/Moderation.hpp"
#include <libwebsockets.h>
#include "utils/Json.hpp"
#include <atomic>
#include <string>

struct Ctx{ std::string req_body; std::string resp; std::atomic<bool> done{false}; std::string api_key; };
static int http_cb(lws* wsi, lws_callback_reasons r, void* user, void* in, size_t len){
  auto* c = (Ctx*)lws_wsi_user(wsi);
  switch(r){
    case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER: {
      unsigned char **p=(unsigned char**)in, *end=(*p)+len;
      std::string auth = "Bearer "+c->api_key;
      lws_add_http_header_by_name(wsi,(const unsigned char*)"Authorization:",(const unsigned char*)auth.c_str(),(int)auth.size(),p,end);
      lws_add_http_header_by_name(wsi,(const unsigned char*)"Content-Type:",(const unsigned char*)"application/json",16,p,end);
      break; }
    case LWS_CALLBACK_CLIENT_HTTP_WRITEABLE:
      lws_write(wsi,(unsigned char*)c->req_body.c_str(),(int)c->req_body.size(),LWS_WRITE_HTTP);
      lws_client_http_body_pending(wsi,0); lws_callback_on_writable(wsi); break;
    case LWS_CALLBACK_RECEIVE_CLIENT_HTTP_READ:
    case LWS_CALLBACK_RECEIVE_CLIENT_HTTP: {
      char* pdata=(char*)in; if(pdata && len>0) c->resp.append(pdata,len); break; }
    case LWS_CALLBACK_COMPLETED_CLIENT_HTTP: c->done=true; break;
    case LWS_CALLBACK_CLIENT_CONNECTION_ERROR: c->done=true; break;
    default: break;
  }
  return 0;
}

ModVerdict Moderation::checkText(const std::string& text, const std::string& api_key, const std::string& model){
  using namespace json; Ctx c; c.api_key=api_key; auto r=obj(); add(r,"model",model.c_str()); add(r,"input",text.c_str()); c.req_body=str(r); free(r);
  lws_context_creation_info info{}; info.port=CONTEXT_PORT_NO_LISTEN; info.options=LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT; static lws_protocols prot[]={{"http",http_cb,sizeof(Ctx),4096},{nullptr,nullptr,0,0}}; info.protocols=prot; auto* context=lws_create_context(&info);
  lws_client_connect_info i{}; i.context=context; i.userdata=&c; i.address="api.openai.com"; i.host=i.address; i.port=443; i.path="/v1/moderations"; i.ssl_connection=LCCSCF_USE_SSL; i.method="POST"; i.protocol="http";
  auto* wsi=lws_client_connect_via_info(&i); (void)wsi;
  while(!c.done) lws_service(context, 50);
  lws_context_destroy(context);
  ModVerdict v; if(c.resp.find("\"flagged\":true")!=std::string::npos){ v.flagged=true; v.reason="flagged"; } return v;
}