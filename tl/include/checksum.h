#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <cstdint>
#include <tuple>
#include <utility>



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



#endif // CHECKSUM_H

