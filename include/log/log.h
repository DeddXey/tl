#ifndef LOG_H
#define LOG_H

#include "app_cfg.h"
#include "loglevels.h"
#include "terminalconfig.h"
#include "utility.h"
#include <algorithm>
#include <array>
#include <cstdint>
#include <type_traits>

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
  w8
};

//============================================================================
template<typename I, bool f>
class LogStream
{
public:
  using Port = typename I::OutStream;

private:
  uint8_t base     = 10;
  uint8_t trailing = 0;
  Port    port;

public:
  constexpr static bool flag = f;

  LogStream() = default;
  //------------------------------------------------------------------------
  void init()
  {
    I::init();
  }

  //------------------------------------------------------------------------
  [[nodiscard]] uint8_t getBase() const
  {
    return base;
  }

  //------------------------------------------------------------------------
  [[nodiscard]] uint8_t getTrailing() const
  {
    return trailing;
  }

  //------------------------------------------------------------------------
  void putChar(const uint8_t byte)
  {
    port.putChar(byte);
  }

  //------------------------------------------------------------------------
  void putRawData(const uint8_t *buf, const uint16_t len)
  {
    for (int i = 0; i < len; ++i) {
      putChar(buf[i]);
    }
  }

  //------------------------------------------------------------------------
  void setBase(const uint8_t base)
  {
    this->base = base;
  }

  //------------------------------------------------------------------------
  void setWidth(const uint8_t width)
  {
    this->trailing = width;
  }

  //------------------------------------------------------------------------
  void irqHandler()
  {
    port.irqHandler();
  }
};

//============================================================================
template<typename I>
class LogStream<I, false>
{
public:
  void putChar([[maybe_unused]] const uint8_t byte) {}
};

template<typename P, bool flag = true, LogLevel ll = AppCfg::log_level>
class LogWrapper
{

  LogStream<P, true> &logger;
  LogStream<P, false> dumb;

public:
  explicit LogWrapper(LogStream<P, true> &lg) : logger{ lg } {}

  //------------------------------------------------------------------------
  auto &always()
  {
    if constexpr ((ll >= LogLevel::always) && flag)
      return logger;
    else
      return dumb;
  }

  //------------------------------------------------------------------------
  auto &error()
  {
    if constexpr ((ll >= LogLevel::error) && flag)
      return logger;
    else
      return dumb;
  }

  //------------------------------------------------------------------------
  auto &warning()
  {
    if constexpr ((ll >= LogLevel::warning) && flag)
      return logger;
    else
      return dumb;
  }

  //------------------------------------------------------------------------
  auto &info()
  {
    if constexpr ((ll >= LogLevel::info) && flag)
      return logger;
    else
      return dumb;
  }

  //------------------------------------------------------------------------
  auto &debug()
  {
    if constexpr ((ll >= LogLevel::debug) && flag)
      return logger;
    else
      return dumb;
  }
};

////========================================================================

//---------------------------------------------------------------------------
///
/// Color
template<typename T>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, const Fg val)
{
  const uint8_t buf[]{ 27, '[', '3', static_cast<uint8_t>(val), 'm' };
  stream.putRawData(buf, sizeof(buf));
  return stream;
}

//---------------------------------------------------------------------------
template<typename T>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, const Bg val)
{
  const uint8_t buf[]{ 27, '[', '4', static_cast<uint8_t>(val), 'm' };
  stream.putRawData(buf, sizeof(buf));
  return stream;
}

//---------------------------------------------------------------------------
template<typename T>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, const Attr val)
{
  const uint8_t buf[]{ 27, '[', static_cast<uint8_t>(val), 'm' };
  stream.putRawData(buf, sizeof(buf));
  return stream;
}

//---------------------------------------------------------------------------
template<typename T>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, Use val)
{
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
  }
  return stream;
}

//----------------------------------------------------------------------------
template<typename T>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, const char *val)
{
  while (*val)
    stream.putChar(*val++);
  return stream;
}

//----------------------------------------------------------------------------
template<typename T, typename S>
LogStream<T, true> &operator<<(LogStream<T, true> &stream, const S val)
{
  std::array<uint8_t, 64> buffer;
  S                       sum = val;

  if constexpr (std::is_signed<S>::value) {
    if (val < 0) {
      stream.putChar('-');
      sum = -sum;
    }
  }

  uint8_t i = 0;
  do {
    int32_t digit;
    digit = sum % stream.getBase();
    if (digit < 0xA)
      buffer[i] = '0' + digit;
    else
      buffer[i] = 'A' + digit - 0xA;
    sum /= stream.getBase();
    ++i;
  } while (sum);

  for (int j = 0; j < stream.getTrailing() - i; ++j) {
    stream.putChar('0');
  }

  for (; i != 0; --i) {
    stream.putChar(buffer[i - 1]);
  }

  return stream;
}

//----------------------------------------------------------------------------
template<typename T, typename... Args>
LogStream<T, false> &operator<<(LogStream<T, false> &stream,
                                [[maybe_unused]] Args... args)
{
  return stream;
}

extern LogStream<ConsoleCfg, USE_DEBUG> serialLog;
extern LogWrapper<ConsoleCfg>           con;

#endif // LOG_H
