#pragma once
#include <cstddef>
#include <type_traits>
#include <utility>

namespace tl {

///
/// \tparam I1
/// \tparam I2
template<typename I1, typename I2>
class Range
{

  I1 begin_;
  I2 end_;

public:

  using value_type = typename I1::value_type;

  constexpr Range(): begin_(), end_() {}

  /// \brief Construct from two iterators
  /// \param begin begin iterator
  /// \param end end iterator
  constexpr Range(const I1 begin, const I2 end) noexcept :
    begin_(begin), end_(end)
  {
  }

  /// \brief Get begin iterator
  /// \return begin iterator
  constexpr auto begin() const noexcept
  {
    return begin_;
  }

  /// \brief Get end iterator
  /// \return end iterator
  constexpr auto end() const noexcept
  {
    return end_;
  }

  constexpr auto is_empty() const noexcept
  {
    return (end_ == begin_);
  }

  constexpr bool operator == (const Range<I1, I2>& other)  const
  {
    return (this->begin_ == other.begin_)&&(this->end_ == other.end_);
  }

  // TODO: add guard of LogStream
//  template<typename T>
//  friend std::enable_if<std::is_same_v<typename I1::value_type, uint8_t>,
//                        LogStream<T, true> &>::type
//  operator<<(LogStream<T, true> &stream, const Range<I1, I2> &range)
//  {
//    stream << Use::hex << Fg::cyan << "Range\n";
//
//    uint8_t cnt = 0;
//
//    for (auto i : range) {
//      if (cnt++ % 16 == 0) {
//        stream.putChar('\n');
//      }
//
//      stream << Use::w2 << i << " ";
//    }
//    stream << Attr::reset << Use::endl;
//    return stream;
//  }





};

/// \brief Make Range from any collection
/// \tparam T the type of collection
/// \param collection
/// \return new collection
template<typename T>
constexpr auto makeRange(T &collection) noexcept
{
  return Range<decltype(collection.begin()), decltype(collection.end())>(
    collection.begin(),
    collection.end());
}

///
/// \tparam T
/// \tparam N
/// \param array
/// \return
template<class T, std::size_t N>
constexpr auto makeRange(T (&array)[N]) noexcept
{
  return Range<T *, T *>(array, array + N);
}

///
/// \tparam T
/// \param ptr
/// \param size
/// \return
template<class T>
constexpr auto makeRange(T *ptr, size_t size) noexcept
{
  return Range<T *, T *>(ptr, ptr + size);
}
} // namespace tl
///
/// \tparam Rng
/// \tparam F
/// \param rng
/// \param c
/// \return
template<typename Rng, typename F>
constexpr auto operator|(Rng &&rng, F &&c)
  -> decltype(c(std::forward<Rng>(rng)))
{
  return c(std::forward<Rng>(rng));
}

