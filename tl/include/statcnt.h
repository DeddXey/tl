#ifndef STATCNT_H
#define STATCNT_H

#include <array>
#include <algorithm>    // std::minmax_element
#include <functional>
#include <numeric>

/// \file

//============================================================================
///
/// \brief Simple statistics counter of slipping window
/// \tparam T the type of measuring items
/// \tparam sz the size of the counting window
///
template <typename T, int sz>
class SlipStatCnt {

    using Type = std::array<T, sz>;

    Type    buffer;
    T       min;
    T       max;
    T       avg;
    typename Type::iterator currIter;
    T       sum;
    bool    mmChanged = false;
    bool    avgChanged = false;

    //------------------------------------------------------------------------
    void recalcMinMax()
    {
        auto result = std::minmax_element (buffer.begin(), buffer.end());

        min = *result.first;
        max = *result.second;
        mmChanged = false;
    }

    //------------------------------------------------------------------------
    void recalcAvg()
    {
        auto avg1 = std::accumulate(buffer.begin(),
                                    buffer.end(),
                                    0.0f);
        avg = avg1 / static_cast<T>(sz);

        avgChanged = false;
    }

public:
    //------------------------------------------------------------------------
    ///
    /// \brief Construct and fill the buffer
    ///
    explicit SlipStatCnt(T value = 0) :
                                    min(value),
                                    max(value),
                                    avg(value),
                                    currIter(buffer.begin()),
                                    sum(value * sz)
    {
        for (auto &i:buffer) {
            i = value;
        }
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Add next value to the buffer
    /// \param value value to add
    ///
    void addValue(T value)
    {
        *currIter = value;
        ++currIter;
        if (currIter == buffer.end())
            currIter = buffer.begin();

        mmChanged = true;
        avgChanged = true;
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Fill the buffer with value
    /// \param value value to fill
    ///
    void setValue(T value)
    {
        for (auto i = buffer.begin(); i != buffer.end(); ++i) {
            *i = value;
        }
        min = value;
        max = value;
        sum = value * sz;
        avg = value;
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Get minimal value in the buffer
    /// \return the value
    ///
    T getMin()
    {
        if (mmChanged) {
            recalcMinMax();
        }

        return min;
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Get maximal value in the buffer
    /// \return the value
    ///
    T getMax()
    {
        if (mmChanged) {
            recalcMinMax();
        }

        return max;
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Get average value in the buffer
    /// \return the value
    ///
    T getAvg()
    {
        if (avgChanged) {
            recalcAvg();
        }
        return (avg);
    }

};

//============================================================================
template <typename T, int sz>
class Fir {

    using Type = std::array<T, sz>;

    Type	buffer;
    const std::array<T, sz> *coeffs;
    typename Type::iterator currIter;


public:

    Fir() :
            currIter(buffer.begin())
    {
        for (auto &i:buffer) {
            i = 0.0;
        }
    }

    //------------------------------------------------------------------------
    void setCoeffs(const std::array<T, sz> *coefficients)
    {
        coeffs = coefficients;
    }

    //------------------------------------------------------------------------
    void addValue(T value)
    {
        *currIter = value;
        ++currIter;
        if (currIter == buffer.end())
            currIter = buffer.begin();
    }

    //------------------------------------------------------------------------
    T getValue()
    {
        auto iter = currIter;
        T sum = 0.0;

        for (auto i: *coeffs) {

            sum = sum + (i * (*iter));

            ++iter;
            if (iter == buffer.end())
                iter = buffer.begin();

        }

        return sum;
    }

};


#endif // STATCNT_H

