#include "audio/AudioIn.hpp"
#include <portaudio.h>
#include <vector>
#include <thread>
#include <atomic>

struct InCtx { PaStream* s=nullptr; int sr=24000; int ch=1; int frame=480; std::atomic<bool> run{false}; AudioIn::OnFrame cb; };
static InCtx g_in;

bool AudioIn::start(int sampleRate, int channels, int frameMs, OnFrame cb){
  g_in.sr = sampleRate; g_in.ch = channels; g_in.frame = (sampleRate*frameMs/1000); g_in.cb = cb;
  if(Pa_Initialize()!=paNoError) return false;
  PaStreamParameters input{}; input.device = Pa_GetDefaultInputDevice(); input.channelCount = channels; input.sampleFormat = paInt16; input.suggestedLatency = 0.03; input.hostApiSpecificStreamInfo = nullptr;
  if(input.device == paNoDevice) return false;
  if(Pa_OpenStream(&g_in.s, &input, nullptr, sampleRate, g_in.frame, paNoFlag, nullptr, nullptr) != paNoError) return false;
  if(Pa_StartStream(g_in.s) != paNoError) return false;
  g_in.run = true;
  std::thread([]{
    std::vector<int16_t> buf(g_in.frame * g_in.ch);
    while(g_in.run){
      auto r = Pa_ReadStream(g_in.s, buf.data(), g_in.frame);
      if(r==paNoError && g_in.cb) g_in.cb(buf.data(), g_in.frame);
    }
  }).detach();
  return true;
}

void AudioIn::stop(){ g_in.run=false; if(g_in.s){ Pa_StopStream(g_in.s); Pa_CloseStream(g_in.s); g_in.s=nullptr; } Pa_Terminate(); }