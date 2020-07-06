#ifndef STATCNT_H
#define STATCNT_H

#include <array>
#include <algorithm>    // std::minmax_element
#include <functional>
#include <numeric>

//============================================================================
template <typename T, int sz>
class SlipStatCnt {

	using Type = std::array<T, sz>;

	Type	buffer;
	T min;
	T max;
	T avg;
	typename Type::iterator currIter;
	T sum;

public:

    //------------------------------------------------------------------------
    SlipStatCnt() :
	    min(0),
	    max(0),
	    avg(0),
	    currIter(buffer.begin()),
	    sum(0)
	{
		for (auto &i:buffer) {
			i = 0;
		}
	}

    //------------------------------------------------------------------------
    explicit SlipStatCnt(T value) :
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
    void addValue(T value)
	{
		*currIter = value;
		++currIter;
		if (currIter == buffer.end())
			currIter = buffer.begin();


		auto result = std::minmax_element (buffer.begin(), buffer.end());

		min = *result.first;
		max = *result.second;

		auto avg1 = std::accumulate(buffer.begin(),
		                      buffer.end(),
		                      0.0);
		avg = avg1 / static_cast<T>(sz);
	}

    //------------------------------------------------------------------------
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
    T getMin() const
	{
		return min;
	}

    //------------------------------------------------------------------------
    T getMax() const
	{
		return max;
	}

    //------------------------------------------------------------------------
    T getAvg()
	{
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

