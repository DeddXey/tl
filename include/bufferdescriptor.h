#pragma once

#include "log.h"

#include <cstdint>

#include <utility>

#ifdef _MSC_VER
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#else
#include <unistd.h>
#endif

///
/// \brief Descriptor of simple memory buffer
///
template<typename M = uint8_t>
class BufferDescriptor
{
  const M *buffer;
  size_t size = 0;

public:

  class iterator;
  friend class iterator;
  class iterator :public std::iterator<
                      std::random_access_iterator_tag,
                      M>
  {
    BufferDescriptor& bd;
    size_t idx;
  public:
    explicit iterator(BufferDescriptor& ibd, size_t _idx) : bd(ibd), idx(_idx) {}
    iterator& operator++() { idx = idx + 1; return *this; }
    iterator operator++(int) { iterator retval = *this; ++(*this); return retval; }
    bool operator==(iterator other) const { return idx == other.idx; }
    bool operator!=(iterator other) const { return !(*this == other); }
    const M& operator*() const { return bd.buffer[idx]; }

    iterator operator + (ssize_t val)
    {
      iterator temp = *this;
      temp.idx = static_cast<size_t>(static_cast<ssize_t>(temp.idx) + val);
      if (static_cast<size_t>(temp.idx) >= bd.size) {
        temp.idx = bd.size;
      }
      return temp;
    }

    iterator operator - (int val)
    {
      iterator temp = *this;
      ssize_t tmp = static_cast<ssize_t>(temp.idx) - val;
      if (tmp < 0) {
        temp.idx = 0;
      }
      else {
        temp.idx = static_cast<size_t>(tmp);
      }
      return temp;
    }

    ptrdiff_t operator - (iterator other)
    {
      ptrdiff_t out = this->idx - other.idx;
      return out;
    }
  };

  iterator begin()
  {
    return iterator(*this, 0);
  }

  iterator end()
  {
    return iterator(*this, size);
  }

  template<typename T>
  explicit BufferDescriptor(T& entity)
    : buffer(reinterpret_cast<uint8_t*>(&entity)), size(sizeof(entity)) {}

  BufferDescriptor(const M *ptr, const size_t len) : buffer(ptr), size(len) {}
  BufferDescriptor() = default;

  template<size_t N>
  explicit BufferDescriptor(M (&array)[N]) noexcept : buffer(array), size(N)
  {
  }

  /// \brief Get BufferDescriptor from discrete parameters
  /// \param buf
  /// \param len
  void fromValues(const M *buf, const uint32_t len)
  {
    this->buffer = buf;
    this->size   = len;
  }

  template<typename T, typename S>
  friend LogStream<T, true> &operator<<(LogStream<T, true>        &stream,
                                        const BufferDescriptor<S> &buffer_descriptor)
  {

      stream << Fg::cyan << Use::hex << Use::w8 << "addr: " << (int)buffer_descriptor.ptr()
             << " length: " << buffer_descriptor.length() << ":" << buffer_descriptor.length() * sizeof(S);

      const auto *ptr8 = reinterpret_cast<const uint8_t *>(buffer_descriptor.buffer);

      for (uint32_t i = 0; i < buffer_descriptor.length() * sizeof(S); ++i) {
          if (i % 16 == 0) {
              stream.putChar('\n');
          }

          stream << Use::w2 << ptr8[i] << " ";
      }
      stream << Attr::reset << Use::endl;
      return stream;
  }

  [[nodiscard]] uint32_t length() const
  {
    return size;
  }

  const M *ptr() const
  {
    return buffer;
  }
};

