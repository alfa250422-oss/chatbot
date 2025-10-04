#pragma once
#include <vector>
#include <cstdint>
class RingBuffer {
public:
    explicit RingBuffer(size_t cap) : buf(cap), r(0), w(0) {}
    size_t write(const uint8_t* p, size_t n);
    size_t read(uint8_t* p, size_t n);
    size_t avail() const; // used bytes
    size_t cap() const { return buf.size(); }
private:
    std::vector<uint8_t> buf; size_t r, w;
};