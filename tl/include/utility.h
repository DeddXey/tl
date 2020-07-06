#ifndef UTILITY_H
#define UTILITY_H

#include "bits.h"
#include <cstdint>
#include <tuple>

#define INLINE
//#ifndef INLINE
//#define INLINE __attribute__((__always_inline__)) inline
//#endif

#ifdef __GNUC__
#define NOP asm ("nop")
#else
#define NOP __asm {nop}
#endif


//#ifndef INLINE
//#define INLINE __attribute__((__always_inline__)) inline
//#endif

#ifndef UNUSED
#define UNUSED __attribute__((unused))
#endif

#define	__packed	__attribute__((__packed__))

///
/// \brief Класс для статических функций утилит
///
//========================================================================
// Register setters and getters section
///
//-----------------------------------------------------------------------
template<typename V, typename T>
INLINE constexpr static uint32_t setBitsR(T field, V val)
{
    return (val << (field[0]));
}

//-----------------------------------------------------------------------
template<typename V, typename T, typename... Args>
INLINE constexpr static uint32_t  setBitsR(T field,
                                           V val,
                                           Args... args)
{
    return (val << (field[0])) | setBitsR(args...);
}

//-----------------------------------------------------------------------
template<typename V, typename T>
INLINE constexpr static uint32_t setMaskR(T field,
                                          V val)
{
    return ((((1 << field[1]) - 1) << field[0]));
}

//-----------------------------------------------------------------------
template<typename V, typename T, typename... Args>
INLINE constexpr static uint32_t  setMaskR(T field,
                                           V val,
                                           Args... args)
{
    return ((((1 << field[1]) - 1) << field[0])) | setMaskR(args...);
}

//-----------------------------------------------------------------------
/// Sets several register fields to values
/// @param reg register to set
/// @param field the register field in library format
/// @param value value to set
template<typename T, typename V, typename... Args>
INLINE constexpr static void setRegister(volatile uint32_t& reg,
                                         const T field,
                                         const V value,
                                         const Args... args)
{
    uint32_t mask = setMaskR(field, value, args...);
    uint32_t val = setBitsR(field, value, args...);

//    cpu::cpsid();
    reg = (reg & (~mask)) | val;
//    cpu::cpsie();
}




//-----------------------------------------------------------------------
///
/// Returns a value of register field
/// @param reg register
/// @param field register field in library format
///
template<typename T>
INLINE constexpr static uint32_t getRegField(volatile uint32_t& reg,
                                             const T field)
{
    uint32_t mask = (((1 << field[1]) - 1) << field[0]); // TODO: optimize?
    return ((reg & mask) >> field[0]);
}

///-----------------------------------------------------------------------
///
/// \brief simpleDelay
/// \param delay delay in cycles
///
void simpleDelay(volatile uint32_t delay);

//------------------------------------------------------------------------
/// \brief Преобразование любого типа в ссылку на unt32_t
///  удобно для работы со структурами битовых полей, их копирования и т.д.
///
template <typename T>
static INLINE volatile uint32_t& asWord(T& what)
{
    return *reinterpret_cast<volatile uint32_t*>(&what);
}

//-----------------------------------------------------------------------
///
/// \brief Converts the pointer to ADC buffer (24 bits MSB first) to int32
/// \param ptr pointer  to buffer
/// \return int32 value
///
int32_t readAdcSample(const uint8_t *ptr);
//-----------------------------------------------------------------------
uint8_t hexLetterToByte(const uint8_t letter);
//-----------------------------------------------------------------------
uint8_t decLetterToByte(const uint8_t letter);
//-----------------------------------------------------------------------
///
/// \brief bcd2Byte
/// \param val
/// \return
///
INLINE constexpr static uint8_t bcd2Byte(uint8_t val)
{
    return ((((val & 0xf0U) >> 4U) * 10U) +
            (val & 0x0fU));
}
//-----------------------------------------------------------------------
INLINE constexpr static int16_t reverse16s(int16_t in)
{
    int16_t out = ((in & 0xffU) << 8U) | ((in >> 8U) & 0xffU);
    return out;
}



#endif // UTILITY_H
