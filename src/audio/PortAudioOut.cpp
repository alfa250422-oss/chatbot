#include "audio/AudioOut.hpp"
#include <portaudio.h>

struct OutCtx { PaStream* s=nullptr; int sr=24000; int ch=1; int frame=480; };
static OutCtx g_out;

bool AudioOut::start(int sampleRate, int channels){
  g_out.sr=sampleRate; g_out.ch=channels; g_out.frame = sampleRate/50; // 20ms
  if(Pa_Initialize()!=paNoError) return false;
  PaStreamParameters output{}; output.device=Pa_GetDefaultOutputDevice(); output.channelCount=channels; output.sampleFormat=paInt16; output.suggestedLatency=0.03; output.hostApiSpecificStreamInfo=nullptr;
  if(output.device==paNoDevice) return false;
  if(Pa_OpenStream(&g_out.s, nullptr, &output, sampleRate, g_out.frame, paNoFlag, nullptr, nullptr)!=paNoError) return false;
  if(Pa_StartStream(g_out.s)!=paNoError) return false;
  return true;
}

void AudioOut::stop(){ if(g_out.s){ Pa_StopStream(g_out.s); Pa_CloseStream(g_out.s); g_out.s=nullptr; } Pa_Terminate(); }

bool AudioOut::play(const int16_t* pcm, size_t samples){
  if(!g_out.s) return false; return Pa_WriteStream(g_out.s, pcm, (unsigned long)samples) == paNoError;
}