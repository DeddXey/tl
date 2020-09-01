#ifndef FILTERS_H
#define FILTERS_H

#include <cstdint>
#include <array>
#include <algorithm>
#include <numeric>


template <typename T, uint16_t len>
class MedianFilter {

    std::array<T, len> buffer;
    std::array<T, len> sortBuffer;

    uint16_t ptr = 0;

public:

    ///
    /// \brief Конструктор по умолчанию
    ///
    MedianFilter()
    {
        std::fill(buffer.begin(), buffer.end(), 0);
        ptr = 0;
    }

    ///
    /// \brief Основная рабочая итерация
    /// \param sample входные данные
    /// \return выходные данные
    ///
    T iteration(const T& sample)
    {
        buffer[ptr++] = sample;

        if (ptr == len)
            ptr = 0;
        sortBuffer = buffer;

        std::sort(sortBuffer.begin(), sortBuffer.end());

        return sortBuffer[static_cast<uint16_t>(len / 2)];
    }

};

template <typename T, uint16_t len>
class MovingAverageFilter {
    std::array<T, len> buffer;
    uint16_t ptr = 0;
public:
    ///
    /// \brief Конструктор по умолчанию
    ///
    MovingAverageFilter()
    {
        std::fill(buffer.begin(), buffer.end(), 0);

    }

    ///
    /// \brief Основная рабочая итерация
    /// \param sample входные данные
    /// \return выходные данные
    ///
    T iteration(const T& sample)
    {
        buffer[ptr++] = sample;

        if (ptr == len)
            ptr = 0;

        auto sum = std::accumulate(buffer.begin(), buffer.end(), 0);
        return sum / len;
    }

};




#endif // FILTERS_H
