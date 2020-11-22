#ifndef VIEW_H
#define VIEW_H

#include <cstdint>
#include <iterator>

namespace tl {
template<typename T>
class ViewIterator
{
  T *ptr;

public:
  using iterator_category = std::random_access_iterator_tag;
  using value_type        = uint16_t;
  using difference_type   = ptrdiff_t ;
  using pointer           = uint16_t *;
  using reference         = uint16_t &;
  explicit ViewIterator(T *p = nullptr) : ptr(p) {}

  ViewIterator &operator++()
  {
    ptr += 1;
    return *this;
  }

  ViewIterator operator++(int)
  {
    ViewIterator retval = *this;
    ++(*this);
    return retval;
  }

  bool operator==(ViewIterator other) const
  {
    return ptr == other.ptr;
  }

  bool operator!=(ViewIterator other) const
  {
    return !(*this == other);
  }

  T &operator*() const
  {
    return *ptr;
  }

  ViewIterator &operator+=(size_t val)
  {
    ptr += val;
    return *this;
  }

  ViewIterator &operator-=(size_t val)
  {
    ptr -= val;
    return *this;
  }

  friend ViewIterator operator+(const ViewIterator &lhs, size_t val)
  {
    return ViewIterator(lhs.ptr + val);
  }

  friend ViewIterator operator+(size_t val, const ViewIterator &rhs)
  {
    return ViewIterator(rhs.ptr + val);
  }

  friend ViewIterator operator-(const ViewIterator &lhs, size_t val)
  {
    return ViewIterator(lhs.ptr - val);
  }

  friend ptrdiff_t operator-(const ViewIterator &lhs, const ViewIterator &rhs)
  {
    return lhs.ptr - rhs.ptr;
  }
};

//------------------------------------------------------------------------------------------------------
///
/// Wrapper class to cast any data to iterable type
/// struct -> array like data etc
/// \tparam T type of array members
template<typename T>
class View
{

  using iterator = ViewIterator<T>;

  const iterator begin_;
  const size_t   size = 0;

public:
  /// No default constructors
  View() = delete;

  /// Constructor
  /// \tparam D source data type
  /// \param data source data
  template<typename D>
  View(D &data) :
    begin_(reinterpret_cast<T *>(&data)), size(sizeof(data) / sizeof(T))
  {
  }

  /// Get pointer to the data begin
  T *getPointer() const noexcept
  {
    return &(*begin_);
  }

  /// Get size of data (the count of the T-type members in array)
  size_t getSize() const noexcept
  {
    return size;
  }

  /// Get the begin iterator of preudocontainer
  ViewIterator<T> begin() const noexcept
  {
    return begin_;
  }

  /// Get the end iterator of preudocontainer
  ViewIterator<T> end() const noexcept
  {
    return begin_ + size;
  }
};


} // namespace tl

#endif // VIEW_H
