#ifndef ADC24TOINT32ITERATOR_H
#define ADC24TOINT32ITERATOR_H


#include "ranges/range.h"
#include <cstdint>
#include <utility>

//============================================================================
template <typename It>
class Adc24ToInt32Iterator {

    It  it;
public:
    using value_type = int32_t;

    Adc24ToInt32Iterator(It iterator) :
        it(iterator) {}

    Adc24ToInt32Iterator(const Adc24ToInt32Iterator& other)
    {
        it = other.it;
    }


    Adc24ToInt32Iterator(Adc24ToInt32Iterator&& other) :
        it(std::move(other.it))

    {

    }

    //------------------------------------------------------------------------
    int32_t operator*() const
    {
        int32_t tmp = *(it) << 24 |
                               (*(it + 1) << 16)|
                               (*(it + 2) << 8);
        return tmp;
    }

    //------------------------------------------------------------------------
    Adc24ToInt32Iterator& operator++()
    {
        it += 3;
        return *this;
    }

    //------------------------------------------------------------------------
    Adc24ToInt32Iterator operator++(int)
    {
        Adc24ToInt32Iterator tmp(*this);
        operator++();
        return tmp;
    }

    //------------------------------------------------------------------------
    bool operator != (const Adc24ToInt32Iterator& other) const
    {
        return (this->it != other.it);
    }

    //------------------------------------------------------------------------
    bool operator == (const Adc24ToInt32Iterator& other) const
    {
        return (!(*this != other));
    }
};

//============================================================================
//template<typename R>
auto adc24ToInt32() noexcept
{
    auto ret  = [](auto ran)
    {
        using ItType = Adc24ToInt32Iterator<decltype((std::begin(ran)))>;

        return Range<ItType, decltype(std::end(ran))>
                (ItType(std::begin(ran)), std::end(ran));
    };

    return ret;
}



#endif // ADC24TOINT32ITERATOR_H
