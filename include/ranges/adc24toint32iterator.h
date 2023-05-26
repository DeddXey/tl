#pragma once

#include "ranges/range.h"
#include <cstdint>
#include <iterator>
#include <span>
#include <utility>

namespace tl {
///
/// \tparam It
template<typename It>
class [[deprecated]] Adc24ToInt32Iterator
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

template<std::ranges::input_range R>
  requires std::ranges::view<R>
class adc24int32_view : public std::ranges::view_interface<adc24int32_view<R>>
{

public:
  explicit adc24int32_view(R&& r) :
    begin_(std::begin(r)), end_(std::end(r))
  {
  }

  class iterator
  {
    std::ranges::iterator_t<R> itr;

  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = int32_t;
    using difference_type   = ptrdiff_t;
    using reference         = int16_t &;

    constexpr explicit iterator(std::ranges::iterator_t<R> i) : itr(i) {}

    constexpr int32_t operator*() const noexcept
    {
      return (*(itr) << 24 | (*(itr + 1) << 16) | (*(itr + 2) << 8)) >> 8;
    }

    constexpr iterator &operator++() noexcept
    {
      itr += 3;
      return *this;
    }

    constexpr iterator operator++(int) noexcept
    {
      const iterator tmp(*this);
      operator++();
      return tmp;
    }

    constexpr bool operator!=(const iterator &other) const noexcept
    {
      return (this->itr != other.itr);
    }

    constexpr bool operator==(const iterator &other) const noexcept
    {
      return (!(*this != other));
    }
  };

  auto begin()
  {
    return begin_;
  }

  auto end()
  {
    return end_;
  }

private:
  iterator begin_;
  iterator end_;
};

} // namespace tl
