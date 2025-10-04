#include "utils/Base64.hpp"
#include <libwebsockets.h>
#include <vector>
#include <cstring>

namespace b64 {
  std::string encode_pcm16(const int16_t* pcm_le, size_t samples){
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(pcm_le);
    const int in_len = (int)(samples * sizeof(int16_t));
    // 计算输出长度（含终止符），lws 会写入 NUL 结尾
    std::vector<char> out(((in_len + 2) / 3) * 4 + 4);
    int n = lws_b64_encode_string((const char*)bytes, in_len, out.data(), (int)out.size());
    if(n < 0) return {};
    return std::string(out.data(), n);
  }

  std::vector<uint8_t> decode(const std::string& s){
    std::vector<uint8_t> out(s.size());
    int n = lws_b64_decode_string(s.c_str(), (char*)out.data(), (int)out.size());
    if(n < 0) return {};
    out.resize(n);
    return out;
  }
}