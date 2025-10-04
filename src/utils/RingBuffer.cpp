#include "utils/RingBuffer.hpp"
size_t RingBuffer::avail() const { return (w >= r) ? (w - r) : (buf.size() - (r - w)); }
size_t RingBuffer::write(const uint8_t* p, size_t n){ size_t wr=0; while(wr<n){ if(avail()+1>=buf.size()) break; buf[w]=p[wr++]; w=(w+1)%buf.size(); } return wr; }
size_t RingBuffer::read(uint8_t* p, size_t n){ size_t rd=0; while(rd<n && avail()>0){ p[rd++]=buf[r]; r=(r+1)%buf.size(); } return rd; }