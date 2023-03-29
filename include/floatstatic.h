#pragma once

#include <cstdint>
#include <algorithm>
#include <iostream>
#include <cmath>

template <typename T, int exp>
struct FloatStatic {
    constexpr static int exponent =  exp;
    T fraction;
};

template <typename T>
struct FloatStaticTraits {
};

template<>
struct FloatStaticTraits<int16_t>{
    using AccumulatorType = int32_t;
    constexpr static int bitsOfT =  16;
};

template<>
struct FloatStaticTraits<int32_t>{
    using AccumulatorType = int64_t;
    constexpr static int bitsOfT =  32;
};

///@brief
///
///@tparam T 
///@tparam FloatStaticTraits<T>::bitsOfT 
///@param val 
///@return constexpr auto 
///
template<typename T, int16_t used = FloatStaticTraits<T>::bitsOfT>
constexpr auto makeFloatStatic(int16_t val)
{
    FloatStatic<T, -(FloatStaticTraits<T>::bitsOfT - used - 1)> result;
    result.fraction = val << (FloatStaticTraits<T>::bitsOfT - used - 1);

    return result;
}

template<typename T>
constexpr auto makeFloatStaticDouble(double val)
{
    FloatStatic<T, 0> result;
    result.fraction = val;

    return result;
}

template <typename T, int e>
constexpr double toDouble(FloatStatic<T, e> a)
{
    double out = a.fraction * pow(2, e);
    return out;
}

template<typename T,
         int e1,
         int e2>
constexpr auto operator+(FloatStatic<T, e1> a1, FloatStatic<T, e2> a2)
{
    FloatStatic<T, std::max(e1,e2) + 1> result;

    constexpr int8_t shift  = a1.exponent - a2.exponent;

    // Приведение мантиссы
    if constexpr (shift > 0) {
        a2.fraction >>= shift;
    }
    else {
        a1.fraction >>= -shift;
    }
    const uint8_t carry = (0x1 & a1.fraction & a2.fraction);
    result.fraction = (a1.fraction >> 1) + (a2.fraction >> 1) + carry;

    return result;
}

template<typename T,
         int e1,
         int e2>
constexpr auto operator-(FloatStatic<T, e1> a1, FloatStatic<T, e2> a2)
{

    FloatStatic<T, std::max(e1,e2)> result;

    constexpr int8_t shift  = a1.exponent - a2.exponent;

    // Приведение мантиссы
    if constexpr (shift > 0) {
        a2.fraction <<= shift;
    }
    else {
        a1.fraction <<= -shift;
    }
    result.fraction = a1.fraction - a2.fraction;

    return result;
}

template<typename T,
         int e1,
         int e2>
constexpr auto operator*(FloatStatic<T, e1> a1, FloatStatic<T, e2> a2)
{
    typename FloatStaticTraits<T>::AccumulatorType acc;

    acc = a1.fraction * a2.fraction;

    FloatStatic<T, (e1 + e2) + FloatStaticTraits<T>::bitsOfT> result;

    result.fraction = acc >> FloatStaticTraits<T>::bitsOfT;

    return result;
}

//template<typename T,
//         int e1,
//         int e2>
//constexpr auto operator/(FloatStatic<T, e1> a1, FloatStatic<T, e2> a2)
//{
//    typename FloatStaticTraits<T>::AccumulatorType acc = a1.fraction
//            << FloatStaticTraits<T>::bitsOfT;
//
//    acc /= a2.fraction;
//
//    FloatStatic<T, (e1 - e2)> result;
//
//    result.fraction = acc >> FloatStaticTraits<T>::bitsOfT;
//
//    return result;
//}



