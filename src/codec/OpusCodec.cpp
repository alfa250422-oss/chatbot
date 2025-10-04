#include "codec/OpusCodec.hpp"
#include <opus/opus.h>
struct OpusCodec::Impl{ OpusEncoder* enc=nullptr; OpusDecoder* dec=nullptr; int sr=24000; int ch=1; };
bool OpusCodec::init(int sampleRate, int channels, int bitrate){ int err=0; impl=new Impl(); impl->sr=sampleRate; impl->ch=channels; impl->enc=opus_encoder_create(sampleRate,channels,OPUS_APPLICATION_VOIP,&err); if(err!=OPUS_OK) return false; opus_encoder_ctl(impl->enc, OPUS_SET_BITRATE(bitrate)); impl->dec=opus_decoder_create(sampleRate,channels,&err); return err==OPUS_OK; }
int OpusCodec::encode(const int16_t* pcm,int samples,std::vector<uint8_t>& out){ out.resize(4000); int n=opus_encoder_encode(impl->enc,pcm,samples,out.data(),(opus_int32)out.size()); if(n>0) out.resize(n); else out.clear(); return n; }
int OpusCodec::decode(const uint8_t* pkt,int len,std::vector<int16_t>& out){ out.resize(impl->sr/50*impl->ch); int n=opus_decoder_decode(impl->dec,pkt,len,out.data(),(int)out.size(),0); if(n>0) out.resize(n*impl->ch); else out.clear(); return n; }
OpusCodec::~OpusCodec(){ if(!impl) return; if(impl->enc) opus_encoder_destroy(impl->enc); if(impl->dec) opus_decoder_destroy(impl->dec); delete impl; }