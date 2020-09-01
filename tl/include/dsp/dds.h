#ifndef DDS_H
#define DDS_H

#include <cstdint>
#include <cmath>

#include"../misc/table.h"

//#include "stm32f4xx.h"

#include "arm_math.h"
#include "arm_common_tables.h"


#ifndef INLINE
#define INLINE __attribute__((__always_inline__)) inline
#endif

// TODO: сделать версию под FAST_MATH_TABLE_SIZE


///
/// \brief Класс DDS
/// \param AccT тип регистра аккумулятора
/// \param tableSize размер таблицы синуса (первый квадрант). Должно быть степенью 2
template <typename AccT, uint16_t tableSize>
class Dds {
    static constexpr double pi = 3.1415926535897;

    ///
    /// \brief Функция заполнения первого квадранта периода синуса
    /// \param idx индекс значения
    /// \return целочисленный синус
    ///
    static constexpr short sin16( unsigned idx )
    {
        return sinf(pi / 2 * idx / tableSize) * 32767;
    }

    static constexpr AccT cosInit = static_cast<AccT>(pow(2, sizeof(AccT) * 8) / 4);    ///< Начальное значение аккумулятора косинуса
    static constexpr uint8_t tableAddressLength = log(tableSize)/log(2);                ///< Разрядность адреса таблицы

    static constexpr uint8_t accShift =
            static_cast<int8_t>(sizeof(AccT) * 8 - tableAddressLength - 2);             ///< Сдвиг для получения адреса из аккумулятора
    AccT    sinAcc = 0;                                                                 ///< Аккумулятор для синуса
    AccT    cosAcc = cosInit;                                                           ///< Аккумулятор для косинуса

    ///
    /// \brief Функция получения значения из таблицы синусов
    ///
    INLINE int16_t sinT(const uint32_t acc)
    {
        static constexpr   Table<int16_t, tableSize> sinTable = tableGenerator<int16_t, tableSize>( sin16 );

        const uint16_t addr = acc >> accShift;  // Адрес для четырёх квадрантов

        uint16_t idx = addr & (~(0x3 << tableAddressLength));

        // Логика разбора по квадрантам
        if (addr & (1 << tableAddressLength))
            idx = tableSize - idx - 1;

        if (addr & (1 << (tableAddressLength + 1)))
            return -sinTable.values[idx];
        else
            return sinTable.values[idx];
    }

public:
    ///
    /// \brief DDS iteration (adding accumulator)
    ///
    INLINE void iteration(AccT M) { sinAcc += M; cosAcc += M; }
    ///
    /// \brief Get sine wave
    ///
    INLINE int16_t getSin() { return sinT(sinAcc); }
    ///
    /// \brief Get cosine wave
    ///
    INLINE int16_t getCos() { return sinT(cosAcc); }
    ///
    /// \brief Получение значения M для заданных частот
    /// \param sampleFreq частота дискретизации
    /// \param freq выходная частота
    ///
    AccT getM(uint32_t sampleFreq, uint32_t freq)
    {
        return static_cast<AccT>(freq * pow(2, sizeof(AccT) * 8) /  sampleFreq);
    }
};



//-----------------------------------------------------------------------------------
///
/// \brief Класс DDS
/// \param AccT тип регистра аккумулятора
/// \param tableSize размер таблицы синуса (первый квадрант). Должно быть степенью 2
template <typename AccT>
class DdsCortex {

    static constexpr AccT cosInit = static_cast<AccT>(pow(2, sizeof(AccT) * 8) / 4);    ///< Начальное значение аккумулятора косинуса
    static constexpr uint8_t tableAddressLength = log(FAST_MATH_TABLE_SIZE)/log(2);                ///< Разрядность адреса таблицы

    static constexpr uint8_t accShift =
            static_cast<int8_t>(sizeof(AccT) * 8 - tableAddressLength);             ///< Сдвиг для получения адреса из аккумулятора
    AccT    sinAcc = 0;                                                                 ///< Аккумулятор для синуса
    AccT    cosAcc = cosInit;                                                           ///< Аккумулятор для косинуса

    ///
    /// \brief Функция получения значения из таблицы синусов
    ///
    INLINE int16_t sinT(const uint32_t acc)
    {

        const uint16_t addr = acc >> accShift;  // Адрес для четырёх квадрантов

        return sinTable_q15[addr];
    }

public:
    ///
    /// \brief DDS iteration (adding accumulator)
    ///
    INLINE void iteration(AccT M) { sinAcc += M; cosAcc += M; }
    ///
    /// \brief Get sine wave
    ///
    INLINE int16_t getSin() { return sinT(sinAcc); }
    ///
    /// \brief Get cosine wave
    ///
    INLINE int16_t getCos() { return sinT(cosAcc); }
    ///
    /// \brief Получение значения M для заданных частот
    /// \param sampleFreq частота дискретизации
    /// \param freq выходная частота
    ///
    AccT getM(uint32_t sampleFreq, uint32_t freq)
    {
        return static_cast<AccT>(freq * pow(2, sizeof(AccT) * 8) /  sampleFreq);
    }
};


#endif //DDS_H
