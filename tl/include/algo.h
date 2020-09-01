#ifndef ALGO_H
#define ALGO_H

#include <limits>
#include <cmath>
#include <iostream>

///
/// Функция возвращает ближайшее число из заданной серии
/// Серия задается в интервале 1..10, множитель автоматически учитывается
/// \todo избавиться от строки  T divider = pow(10, floor(log10(aval)));
template <typename T, typename M>
constexpr T getNearestFromSeries(const T val, const M& arr) noexcept
{
    T len = std::numeric_limits<T>::max();
    T out = 1;
    T aval = std::abs(val);
    T divider = aval?pow(10, floor(log10(aval + 1e-20))):1.0;
    T val10 = aval / divider;

    for (auto a: arr) {
        T l = std::abs(a - val10);
        if ( l < len) {
            len = l;
            out = a;
        }
    }
    return out * divider;
}



#endif // ALGO_H
