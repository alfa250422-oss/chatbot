#pragma once
#include <cstddef>
#include <cstdint>

class Vad {
public:
  bool isSpeech(const int16_t* frame, size_t samples);
};