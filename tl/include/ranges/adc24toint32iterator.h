#ifndef ADC24TOINT32ITERATOR_H
#define ADC24TOINT32ITERATOR_H

#include "ranges/range.h"
#include <cstdint>
#include <iterator>
#include <span>
#include <utility>

template<typename It>
class Adc24ToInt32Iterator
{

  It it;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = int32_t;
  using difference_type   = ptrdiff_t;
  using reference         = uint16_t &;

  constexpr Adc24ToInt32Iterator(It iterator) noexcept :
    it(iterator){};

  constexpr Adc24ToInt32Iterator(const Adc24ToInt32Iterator &other) noexcept
  {
    it = other.it;
  }

  constexpr Adc24ToInt32Iterator(Adc24ToInt32Iterator &&other) noexcept :
    it(std::move(other.it))
  {
  }

  constexpr int32_t operator*() const  noexcept
  {
    int32_t tmp = *(it) << 24 | (*(it + 1) << 16) | (*(it + 2) << 8);
    return tmp;
  }

  constexpr Adc24ToInt32Iterator &operator++() noexcept
  {
    it += 3;
    return *this;
  }

  constexpr Adc24ToInt32Iterator operator++(int) noexcept
  {
    Adc24ToInt32Iterator tmp(*this);
                         operator++();
    return tmp;
  }

  constexpr bool operator!=(const Adc24ToInt32Iterator &other) const noexcept
  {
    return (this->it != other.it);
  }

  constexpr bool operator==(const Adc24ToInt32Iterator &other) const noexcept
  {
    return (!(*this != other));
  }
};

// template<typename R>
constexpr auto adc24ToInt32() noexcept
{
  auto ret = [](auto ran) {
    using DType  = decltype((std::begin(ran)));
    using ItType = Adc24ToInt32Iterator<DType>;

    //    return std::span<typename ItType::value_type>(ItType(std::begin(ran)),
    //                                                  std::end(ran));
    return Range<ItType, decltype(std::end(ran))>(ItType(std::begin(ran)),
                                                  std::end(ran));
  };

  return ret;
}

#endif // ADC24TOINT32ITERATOR_H
