#pragma once
#include <cstdint>
#include <functional>

class AudioIn {
public:
  using OnFrame = std::function<void(const int16_t* pcm, size_t samples)>; // 默认 20ms 帧
  bool start(int sampleRate = 24000, int channels = 1, int frameMs = 20, OnFrame cb = {});
  void stop();
};