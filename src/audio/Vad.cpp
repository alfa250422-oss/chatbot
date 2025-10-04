#include "audio/Vad.hpp"
#include <cmath>

bool Vad::isSpeech(const int16_t* f, size_t n){
  double energy=0; int zc=0; for(size_t i=1;i<n;i++){ energy += f[i]*f[i]; if((f[i]>0)!=(f[i-1]>0)) zc++; }
  energy/=n; return energy>7e6 && zc>6; // 24kHz 粗阈值（仅演示）
}