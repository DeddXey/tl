#ifndef OSTREAM_H
#define OSTREAM_H

#include <cstdint>
#include <string_view>
#include <array>
#include <algorithm>
#include "utility.h"

/// Foreground terminal colors
enum class Fg:uint8_t {
    black = '0', ///< Черный
    red = '1',
    green = '2',
    yellow = '3',
    blue = '4',
    magenta = '5',
    cyan = '6',
    white = '7',
    default_ = '9'
};

/// Background terminal colors
enum class Bg:uint8_t {
    black = '0', ///< Черный
    red = '1',
    green = '2',
    yellow = '3',
    blue = '4',
    magenta = '5',
    cyan = '6',
    white = '7',
    default_ = '9'
};

/// Terminal effects
enum class Attr:uint8_t {
    reset = '0',
    bright = '1',
    dim = '2',
    standout = '3',
    undercore = '4',
    blink = '5',
    reverse = '7',
    hidden = '8'
};

///Modificators
enum class Use:uint8_t {
    dec,
    hex,
    endl,
    tab,
    w0,
    w2,
    w4,
    w8
};


template<typename PutFunc, bool flag=true>
class Ostream
{
    uint8_t base = 10;
    uint8_t trailing = 0;

    constexpr static uint8_t bufferSize = 20;
    constexpr static uint8_t bufferFrom = 10;

public:
    PutFunc putChar;


    explicit Ostream(PutFunc putFunc) : putChar(putFunc) {}


    //------------------------------------------------------------------------
    template<typename T>
    void putNumber(T num)
    {
        if constexpr (flag) {
            for (const auto i : tl::num2Array(num, base, trailing))
                putChar(i);
        }
    }

    //------------------------------------------------------------------------
    void putString(std::string_view str)
    {
        if constexpr (flag) {
            for (const auto i: str) {
                putChar(i);
            }
        }
    }
    //------------------------------------------------------------------------
    template <typename T>
    void putRawData(const T& str)
    {
        if constexpr (flag) {
            for (const auto i: str) {
                putChar(i);
            }
        }
    }
    //------------------------------------------------------------------------
    void setBase(const uint8_t base)
    {
        if constexpr (flag) {
            this->base = base;
        }
    }

    //------------------------------------------------------------------------
    void setWidth(const uint8_t width)
    {
        if constexpr (flag) {
            this->trailing = width;
        }
    }
};


template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const char* val)
{
    stream.putString(val);
    return stream;
}

template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const int32_t val)
{
    stream.putNumber(val);
    return stream;
}

template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Fg val)
{
    const uint8_t buf[] {27, '[', '3', static_cast<uint8_t>(val), 'm'};
    stream.putRawData(buf);
    return stream;
}

template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Bg val)
{
    const uint8_t buf[] {27, '[', '4', static_cast<uint8_t>(val), 'm'};
    stream.putRawData(buf, sizeof (buf));
    return stream;
}

template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, const Attr val)
{
    const uint8_t buf[] {27, '[', static_cast<uint8_t>(val), 'm'};
    stream.putRawData(buf, sizeof (buf));
    return stream;
}

template <typename T, bool flag>
Ostream<T, flag>& operator<<(Ostream<T, flag>& stream, Use val)
{
    switch (val) {
    case Use::dec:
        stream.setBase(10);
        break;
    case Use::hex:
        stream.setBase(16);
        break;
    case Use::endl:
        //	stream.putChar('\n');
        stream.putChar('\r');
        break;
    case Use::tab:
        stream.putChar('\t');
        break;
    case Use::w0:
        stream.setWidth(0);
        break;
    case Use::w2:
        stream.setWidth(2);
        break;
    case Use::w4:
        stream.setWidth(4);
        break;
    case Use::w8:
        stream.setWidth(8);
        break;
    }
    return stream;
}

#endif // OSTREAM_H
