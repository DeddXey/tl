#ifndef LOG_H
#define LOG_H

#include <array>
#include <cstdint>


/// \brief Levels of logging
enum class LogLevel : uint8_t
{
  none    = 0,
  always  = 1,
  error   = 2,
  warning = 3,
  info    = 4,
  debug   = 5,
  usb = 6
};

enum class Fg : uint8_t
{
  black    = '0', ///< Черный
  red      = '1',
  green    = '2',
  yellow   = '3',
  blue     = '4',
  magenta  = '5',
  cyan     = '6',
  white    = '7',
  default_ = '9'
};

enum class Bg : uint8_t
{
  black    = '0', ///< Черный
  red      = '1',
  green    = '2',
  yellow   = '3',
  blue     = '4',
  magenta  = '5',
  cyan     = '6',
  white    = '7',
  default_ = '9'
};

enum class Attr : uint8_t
{
  reset     = '0',
  bright    = '1',
  dim       = '2',
  standout  = '3',
  undercore = '4',
  blink     = '5',
  reverse   = '7',
  hidden    = '8'
};

enum class Use : uint8_t
{
  dec,
  hex,
  endl,
  tab,
  w0,
  w2,
  w4,
  w8,
  sign
};

/// \brief An simple output stream implementation
/// \tparam OutStream static class represented the output port
/// \tparam flag enable flag. If false no code generated
template<typename OutStream, bool flag>
class LogStream
{
public:
  using Port                    = OutStream;
  constexpr static bool enabled = flag;

private:
  uint8_t base     = 10;
  uint8_t trailing = 0;
  bool    signed_values = false;
  Port    port;

public:
  LogStream() = default;

  [[nodiscard]] uint8_t getBase() const
  {
    return base;
  }

  [[nodiscard]] uint8_t getTrailing() const
  {
    return trailing;
  }

  void putChar(const uint8_t byte)
  {
    if constexpr (flag) {
      port.putChar(byte);
    }
  }

  template<typename B>
  void putRawData(const B &buf)
  {
    if constexpr (flag) {
      for (auto chr : buf) {
        putChar(chr);
      }
    }
  }

  void setBase(const uint8_t value)
  {
    this->base = value;
  }

  void setWidth(const uint8_t width)
  {
    this->trailing = width;
  }

  void process()
  {
    port.irqHandler();
  }
  void set_signed_values(bool val)
  {
    signed_values = val;
  }

};

template<typename P, LogLevel ll = LogLevel::none>
class LogWrapper
{

  LogStream<typename P::out_stream, true>  logger;
  LogStream<typename P::out_stream, false> dumb;

public:
  LogWrapper() = default;

  void init()
  {
    P::init();
  }

  auto &always()
  {
    if constexpr (ll >= LogLevel::always) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  auto &error()
  {
    if constexpr (ll >= LogLevel::error) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  auto &warning()
  {
    if constexpr (ll >= LogLevel::warning) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  auto &info()
  {
    if constexpr (ll >= LogLevel::info) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  auto &debug()
  {
    if constexpr (ll >= LogLevel::debug) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  auto &usb()
  {
    if constexpr (ll >= LogLevel::usb) {
      return logger;
    }
    else {
      return dumb;
    }
  }

  void process()
  {
    logger.process();
  }
};

/// Color
template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const Fg val)
{
  if constexpr (stream.enabled) {
    const uint8_t buf[]{ 27, '[', '3', static_cast<uint8_t>(val), 'm' };
    stream.putRawData(buf);
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const Bg val)
{
  if constexpr (stream.enabled) {
    const uint8_t buf[]{ 27, '[', '4', static_cast<uint8_t>(val), 'm' };
    stream.putRawData(buf);
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const Attr val)
{
  if constexpr (stream.enabled) {

    const uint8_t buf[]{ 27, '[', static_cast<uint8_t>(val), 'm' };
    stream.putRawData(buf);
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, Use val)
{
  if constexpr (stream.enabled) {

    switch (val) {
    case Use::dec:
      stream.setBase(10);
      break;
    case Use::hex:
      stream.setBase(16);
      break;
    case Use::endl:
      //		stream.putChar('\r');
      stream.putChar('\n');
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
    case Use::sign:
      stream.set_signed_values(true);
      break;
    }
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const char *val)
{
  if constexpr (stream.enabled) {

    while (*val) {
      stream.putChar(*val++);
    }
  }
  return stream;
}



template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const uint32_t val)
{
  if constexpr (stream.enabled) {

    std::array<uint8_t, 64> buffer{};
    uint32_t                      sum = val;

    uint8_t i = 0;
    do {
      uint8_t digit = 0;
      digit         = static_cast<uint8_t>(sum % stream.getBase());
      if (digit < 0xA) {
        buffer[i] = '0' + digit;
      }
      else {
        buffer[i] = 'A' + digit - 0xA;
      }
      sum /= stream.getBase();
      ++i;
    } while (sum);

    for (int j = 0; j < stream.getTrailing() - i; ++j) {
      stream.putChar('0');
    }

    for (; i != 0; --i) {
      stream.putChar(buffer[i - 1]);
    }
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const int32_t val)
{
  if constexpr (stream.enabled) {

//    std::array<uint8_t, 64> buffer{};
    int32_t                      sum = val;

    if (val < 0) {
      stream.putChar('-');
      sum = -sum;
    }

    return operator<<(stream, static_cast<uint32_t> (sum));

//    uint8_t i = 0;
//    do {
//      int32_t digit = 0;
//      digit         = sum % stream.getBase();
//      if (digit < 0xA) {
//        buffer[i] = '0' + digit;
//      }
//      else {
//        buffer[i] = 'A' + digit - 0xA;
//      }
//      sum /= stream.getBase();
//      ++i;
//    } while (sum);
//
//    for (int j = 0; j < stream.getTrailing() - i; ++j) {
//      stream.putChar('0');
//    }
//
//    for (; i != 0; --i) {
//      stream.putChar(buffer[i - 1]);
//    }
  }
  return stream;
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const  uint8_t val)
{
  return operator<<(stream, static_cast<uint32_t> (val));
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const  uint16_t val)
{
  return operator<<(stream, static_cast<uint32_t> (val));
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const  int8_t val)
{
  return operator<<(stream, static_cast<int32_t> (val));
}

template<typename T, bool f>
LogStream<T, f> &operator<<(LogStream<T, f> &stream, const  int16_t val)
{
  return operator<<(stream, static_cast<int32_t> (val));
}


//template<typename T, typename S, bool f>
//LogStream<T, f> &operator<<(LogStream<T, f> &stream, const S val)
//{
//  if constexpr (stream.enabled) {
//
//    std::array<uint8_t, 64> buffer{};
//    S                       sum = val;
//
//    if constexpr (std::is_signed<S>::value) {
//      if (val < 0) {
//        stream.putChar('-');
//        sum = -sum;
//      }
//    }
//
//    uint8_t i = 0;
//    do {
//      int32_t digit = 0;
//      digit         = sum % stream.getBase();
//      if (digit < 0xA) {
//        buffer[i] = '0' + digit;
//      }
//      else {
//        buffer[i] = 'A' + digit - 0xA;
//      }
//      sum /= stream.getBase();
//      ++i;
//    } while (sum);
//
//    for (int j = 0; j < stream.getTrailing() - i; ++j) {
//      stream.putChar('0');
//    }
//
//    for (; i != 0; --i) {
//      stream.putChar(buffer[i - 1]);
//    }
//  }
//  return stream;
//}


#endif // LOG_H
