#pragma once
#include <vector>
#include <cstdint>
class OpusCodec {
public:
  bool init(int sampleRate = 24000, int channels = 1, int bitrate = 16000);
  int encode(const int16_t* pcm, int samples, std::vector<uint8_t>& out);
  int decode(const uint8_t* pkt, int len, std::vector<int16_t>& out);
  ~OpusCodec();
private:
  struct Impl; Impl* impl=nullptr;
};