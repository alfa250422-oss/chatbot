#pragma once
#include <cstdint>
class AudioOut {
public:
  bool start(int sampleRate = 24000, int channels = 1);
  void stop();
  bool play(const int16_t* pcm, size_t samples); // 阻塞写入 20ms 帧
};