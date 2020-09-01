#ifndef FIR_H
#define FIR_H

#include <stdint.h>
//#include <iostream>

#ifndef INLINE
#define INLINE __attribute__((__always_inline__)) inline
#endif


template <uint16_t size>
class Fir16
{
    int16_t*        coeffs;             ///< Указатель на массив коэффициентов
    int16_t         fltBuf[size];       ///< Буфер отсчётов
    int16_t         bufIndex = 0;       ///< Индекс текущего элемента вставки
    int16_t         corrCoeff = 1;      ///< Коэффициент, корректирующий усиление

public:
    ///
    /// \brief Fir
    /// \param coeffs
    /// \param corrCoeff
    ///

    Fir16()
    {

    }


    void setCoeffs(int16_t* coeffs, int16_t corrCoeff = 1)
    {
        this->coeffs = coeffs;
        this->corrCoeff = corrCoeff;

        // Очистка коэффициентов фильтра
        for(int i  = 0 ; i < size; ++i)
            fltBuf[i] = 0;
    }
    ///
    /// \brief filter
    /// \param value
    /// \return
    ///
    INLINE int16_t filter(int16_t value)
    {
        fltBuf[bufIndex] = value;
        volatile int32_t acc = 0;

        for (int16_t i = size-1; i >=0; --i) {

            bufIndex = (bufIndex + 1) % size;
//            auto volatile tmp1 = *(coeffs + i);
            acc += *(coeffs + i) * fltBuf[bufIndex];
        }

        ++bufIndex;
        bufIndex %= size;

        acc = acc / size / corrCoeff;
        volatile int16_t out = static_cast<int16_t>(acc);
        return out;
    }
};




#endif //FIR_H
