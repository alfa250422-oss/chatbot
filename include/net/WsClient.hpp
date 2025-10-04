#pragma once
#include <string>
#include <functional>
#include <vector>
struct lws_context; struct lws;
class WsClient {
public:
  using OnOpen=std::function<void()>;
  using OnClose=std::function<void(int)>;
  using OnText=std::function<void(const std::string&)>;
  using OnBinary=std::function<void(const uint8_t*,size_t)>;
  bool connect(const std::string& url,const std::vector<std::pair<std::string,std::string>>& headers);
  bool sendText(const std::string& s);
  bool sendBin(const uint8_t* p, size_t n);
  void poll(int timeout_ms=10); void close(); ~WsClient();
  void setOnOpen(OnOpen cb){onOpen=std::move(cb);}
  void setOnClose(OnClose cb){onClose=std::move(cb);}
  void setOnText(OnText cb){onText=std::move(cb);}
  void setOnBinary(OnBinary cb){onBinary=std::move(cb);}

  // 回调触发的安全封装
  void emitOpen(); void emitClose(int);
  void emitText(const std::string&); void emitBinary(const uint8_t*, size_t);
private:
  lws_context* ctx=nullptr; lws* wsi=nullptr; OnOpen onOpen; OnClose onClose; OnText onText; OnBinary onBinary; 
};