#ifndef ADC24TOINT32ITERATOR_H
#define ADC24TOINT32ITERATOR_H

#include "ranges/range.h"
#include <cstdint>
#include <iterator>
#include <span>
#include <utility>

namespace tl {
///
/// \tparam It
template<typename It>
class Adc24ToInt32Iterator
{
  It it;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = int32_t;
  using difference_type   = ptrdiff_t;
  using reference         = uint16_t &;

  ///
  /// \param iterator
  constexpr Adc24ToInt32Iterator(It iterator) noexcept : it(iterator){};

  ///
  constexpr int32_t operator*() const noexcept
  {
    return *(it) << 24 | (*(it + 1) << 16) | (*(it + 2) << 8);
  }

  ///
  /// \return
  constexpr Adc24ToInt32Iterator &operator++() noexcept
  {
    it += 3;
    return *this;
  }

  ///
  /// \return
  constexpr Adc24ToInt32Iterator operator++(int) noexcept
  {
    const Adc24ToInt32Iterator tmp(*this);
                               operator++();
    return tmp;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator!=(const Adc24ToInt32Iterator &other) const noexcept
  {
    return (this->it != other.it);
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator==(const Adc24ToInt32Iterator &other) const noexcept
  {
    return (!(*this != other));
  }
};

///
/// \return
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
} // namespace tl

#endif // ADC24TOINT32ITERATOR_H
