#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <cstdint>
#include <tuple>
#include <utility>
#include "view.h"


//------------------------------------------------------------------------
template<typename T>
auto fletcher8Checksum(T begin, T end)
{
    uint8_t a = 0;
    uint8_t b = 0;

    for (auto i = begin; i != end; ++i) {
        a += *i;
        b = b + a;
    }

    return std::make_tuple(a, b);
}

//------------------------------------------------------------------------
class Fletcher8Checksum {

    uint8_t a = 0;
    uint8_t b = 0;

public:
    Fletcher8Checksum() : a(0), b(0) {}
    void reset();

    uint8_t getB() const
    {
        return b;
    }

    uint8_t getA() const
    {
        return a;
    }

    void addByte(const uint8_t byte);
};

//------------------------------------------------------------------------
template<typename T>
constexpr uint8_t Crc8_16(const T &c) {
    uint8_t crc = 0xFF;
    // unsigned int i;

    for (auto &a: c) {
        crc ^= ((a >> 8U) & 0xffU);
        for (auto i = 0U; i < 8U; i++)
            crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
        crc ^= (a & 0xffU);
        for (auto i = 0U; i < 8; i++)
            crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
    }

    return crc;
}

//------------------------------------------------------------------------
template<typename T>
constexpr uint8_t Crc8(const T &c) {
    uint8_t crc = 0xFF;
    // unsigned int i;

    for (auto &a: c) {
        crc ^= ((a >> 8U) & 0xffU);
        for (auto i = 0U; i < 8U; i++)
            crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
        crc ^= (a & 0xffU);
        for (auto i = 0U; i < 8; i++)
            crc = crc & 0x80U ? (crc << 1U) ^ 0x31U : crc << 1U;
    }

    return crc;
}





#endif // CHECKSUM_H

