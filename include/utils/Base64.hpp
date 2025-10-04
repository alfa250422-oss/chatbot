#pragma once
#include <string>
#include <vector>
namespace b64 {
  std::string encode_pcm16(const int16_t* pcm_le, size_t samples);
  std::vector<uint8_t> decode(const std::string& b64);
}