#pragma once
#include <string>
#include <functional>
#include <vector>
#include <cstdint>
#include "net/WsClient.hpp"

struct RealtimeConfig {
  std::string model = "gpt-4o-realtime-preview-2024-12-17";
  std::string api_key; // 从环境变量读取
  std::string instructions = "你是一位温暖耐心的儿童伙伴，用短句回答并以问题结尾。";
  bool server_vad = true;
};

class RealtimeClient {
public:
  using OnText = std::function<void(const std::string&)>;
  using OnAudio = std::function<void(const int16_t*, size_t)>;

  bool connect(const RealtimeConfig& cfg);
  void sendAudioPCM16(const int16_t* pcm, size_t samples); // base64(PCM16)
  void commitInput();
  void createResponse();
  void poll(int ms=10);
  void close();

  void setOnText(OnText cb){ onText = std::move(cb);} 
  void setOnPcm(OnAudio cb){ onPcm = std::move(cb);} 
private:
  WsClient ws; RealtimeConfig rc; OnText onText; OnAudio onPcm; std::string textBuf;
  std::string makeSessionUpdate();
  void handleServerEvent(const std::string& jsonText);
};