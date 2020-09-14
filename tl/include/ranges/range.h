#ifndef RANGE_H
#define RANGE_H

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
  ///
  /// \param begin
  /// \param end
  constexpr Range(I1 begin, I2 end) noexcept : begin_(begin), end_(end) {}

  ///
  /// \return
  constexpr auto begin() const noexcept
  {
    return begin_;
  }

  ///
  /// \return
  constexpr auto end() const noexcept
  {
    return end_;
  }
};

///
/// \tparam T
/// \param collection
/// \return
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

} // namespace tl

#endif // RANGE_H
