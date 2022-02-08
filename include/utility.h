#ifndef _UTILITY_H
#define _UTILITY_H
/// \file
#include <array>
#include <cstdint>
//#include "gcem.hpp"
#include "instructions.h"

#include "tlcfg.h"

//#ifdef __GNUC__
//#define NOP_CMD asm ("nop")
//#else
//#define NOP
//#endif

/// \defgroup reg_manip Register manipulations

/// Namespace for tl library
namespace tl {

/// \brief Установка группы битов в заданное значение
/// \param mult Длина группы битов
/// \param val Значение группы битов
/// \param pos позиция группы битов
/// \return слово с установленной группой битов
/// Позиция группы устанавливается так, что находится внутри 32-разрядного числа
template<typename M, typename V, typename T>
constexpr uint32_t setBitGroup(const M mult, const V val, const T pos)
{
  //    volatile uint16_t shift = ((pos & (32 / mult - 1)) * mult);
  return (val << ((pos & (32L / mult - 1)) * mult));
}

/// \brief Установка групп битов в заданное значение
/// \param mult Длина группы битов
/// \param val Значение группы битов
/// \param pos позиция очередной группы битов
/// \param args остальные отмеченные позиции
/// \return слово с установленными группами битов
/// Позиция группы устанавливается так, что находится внутри 32-разрядного числа
template<typename M, typename V, typename T, typename... Args>
constexpr uint32_t
setBitGroup(const M mult, const V val, const T pos, const Args... args)
{
  return (val << ((pos & (32L / mult - 1)) * mult))
         | setBitGroup(mult, val, args...);
}

///
/// \ingroup reg_manip
/// \brief Finite function for recursion
/// \return zero
///
constexpr uint32_t setBits()
{
  return 0;
}

///
/// \ingroup reg_manip
/// \brief Sets bits in uint32 word
/// \tparam T the type of the  bitfield descriptor
/// \tparam V the type of the bitfield
/// \tparam Args... another args type
/// \param field bitfield descriptor: the offset and the width of the
/// bitfield. It should be the array length of two. The first element is the
/// offset of the bitfield and the second element is it's width
/// \param val bits to set
/// \param args another args
/// \return filled word
template<typename T, typename V, typename... Args>
constexpr uint32_t setBits(const T field, const V val, Args... args)
{
  return (val << (field[0]) | setBits(args...));
}

///
/// \brief Finite function for recursion
/// \return zero
///
constexpr uint32_t setMasks()
{
  return 0;
}

///
/// \ingroup reg_manip
/// \brief Sets masks (full bitfields) in uint32 word
/// \tparam T the type of the bitfield descriptor
/// \tparam V the type of the bitfield
/// \tparam Args... another args type
/// \param field bitfield descriptor: the offset and the width of the
/// bitfield. It should be the array length of two. The first element is the
/// offset of the bitfield and the second element is it's width
/// \param val bits to set (really not used)
/// \param args another args
/// \return filled word
template<typename T, typename V, typename... Args>
constexpr uint32_t
setMasks(const T field, [[maybe_unused]] const V val, Args... args)
{
  return (((1 << field[1]) - 1) << (field[0]) | setMasks(args...));
}

///
/// \ingroup reg_manip
/// \brief Sets several register fields to values
/// \tparam T the type of the bitfield descriptor
/// \tparam V the type of the bitfield
/// \tparam Args... another args type
/// \param reg register to set
/// \param field bitfield descriptor: the offset and the width of the
/// bitfield. It should be the array length of two. The first element is the
/// offset of the bitfield and the second element is it's width
/// \param value value to set
/// \param args another args
template<typename T, typename V, typename... Args>
constexpr void setRegister(volatile uint32_t &reg,
                           const T            field,
                           const V            value,
                           const Args... args)
{
  uint32_t mask = setMasks(field, value, args...);
  uint32_t val  = setBits(field, value, args...);

  reg = (reg & (~mask)) | val;
}

///
/// \ingroup reg_manip
/// \brief Returns a value of register field
/// \tparam T the type of the bitfield descriptor
/// \param reg register
/// \param field bitfield descriptor: the offset and the width of the
/// bitfield. It should be the array length of two. The first element is the
/// offset of the bitfield and the second element is it's width
/// \return the value of the field
// TODO: add several fields
template<typename T>
constexpr uint32_t getRegField(volatile uint32_t &reg, const T field)
{
  uint32_t mask = (((1 << field[1]) - 1) << field[0]); // TODO: optimize?
  return ((reg & mask) >> field[0]);
}

template<typename... T>
constexpr bool isRegFieldsSet(volatile uint32_t &reg, const T... fields)
{
  auto l = [](auto f) { return ((1 << f[1]) - 1) << f[0]; };

  uint32_t mask = (l(fields) | ...); // TODO: optimize?
  return (reg & mask);
}

///
/// \brief Converts the pointer to ADC buffer (24 bits MSB first) to int32
/// \param ptr pointer  to buffer
/// \return int32 value
///
constexpr int32_t readAdcSample(const uint8_t *ptr)
{
  int32_t tmp = *(ptr) << 24 | (*(ptr + 1) << 16) | (*(ptr + 2) << 8);
  return tmp >> 8;
}

///
/// \brief Converts one symbol of hex character to it's value
/// \param letter The symbol of hex character 0 - f
/// \return the value of this symbol
///
constexpr uint8_t hexLetterToByte(const uint8_t letter)
{
  uint8_t out = 0;

  if ((letter >= 0x30) && letter <= 0x39) {
    out = letter - 0x30;
  }
  else {
    const uint8_t normalizedLetter = letter & 0x4f;

    if ((normalizedLetter >= 0x41) && normalizedLetter <= 0x46) {
      out = normalizedLetter - 0x41 + 0x0A;
    }
  }
  return out;
}

///
/// \brief Converts one symbol of decimal character to it's value
/// \param letter The symbol of hex character 0 - 9
/// \return the value of this symbol
///
constexpr uint8_t decLetterToByte(const uint8_t letter)
{
  uint8_t out = 0;

  if ((letter >= 0x30) && letter <= 0x39) {
    out = letter - 0x30;
  }

  return out;
}

///
/// \brief Converts bcd value to one byte 0x23 -> 23
/// \param val input bcd value
/// \return the result
///
constexpr uint8_t bcd2Byte(uint8_t val)
{
  return ((((val & 0xf0U) >> 4U) * 10U) + (val & 0x0fU));
}

///
/// \brief Swaps bytes in one 16bits word
/// \param val input 16 bits word
/// \return the output 16 bits word
///
constexpr int16_t reverse16s(int16_t in)
{
  int16_t out = ((in & 0xffU) << 8U) | ((in >> 8U) & 0xffU);
  return out;
}

//------------------------------------------------------------------------
///
/// Converts the number to the byte array of symbols
/// \tparam T the type of the number
/// \param number the input number
/// \param base conversion base
/// \param width the minimal width of the result string (without the sign).
/// If the result is shotter it appends with zeros.
///
// constexpr uint32_t n2TBufferSize = 30;
template<typename T>
constexpr std::array<uint8_t, n2TBufferSize>
num2Array(const T number, uint8_t base = 10, uint8_t width = 0) noexcept
{

  const bool minus = (number < 0);
  T          num   = minus ? (-number) : number;

  std::array<uint8_t, n2TBufferSize> buffer{};
  auto                               i = 0U;

  for (; num != 0; ++i) {
    const int32_t digit = num % base;
    if (digit < 0xA)
      buffer[i] = '0' + digit;
    else
      buffer[i] = 'A' + digit - 0xA;
    num /= base;
  }

  while ((i) < width) {
    buffer[i++] = '0';
  }

  if (minus)
    buffer[i++] = '-';
  buffer[i] = '\0';

  for (auto i1 = 0U, i2 = i - 1U; i1 < i2; ++i1, --i2) {
    auto const t = buffer[i1];
    buffer[i1]   = buffer[i2];
    buffer[i2]   = t;
  }

  return buffer;
}

template<typename F, typename D>
bool isFloatingSame(F f, D d)
{
  constexpr F epsilon = 1e-10;
  return ((fabs(f - d) < epsilon));
}

inline void simpleDelay(const uint32_t delay)
{
  for (uint32_t i = 0; i < delay; ++i) {
    uint32_t a;
    ++a;
    cpu::nop();
  }
} // noinline НЕ ТРОГАТЬ!

template<typename T>
uint32_t &asWord(T &what)
{
  return *reinterpret_cast<uint32_t *>(&what);
}

template<typename T, typename S=uint32_t>
S to_index(T what)
{
  return static_cast<S>(what);
}


} // namespace tl

#endif // _UTILITY_H
