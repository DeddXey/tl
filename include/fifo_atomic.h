#ifndef FIFO_ATOMIC_H
#define FIFO_ATOMIC_H

//#include "dbg.h"
#include <array>
#include <atomic>
#include <mutex>
#include <optional>

namespace tl {


/// Iterator for fifo ring buffer
template<typename T, uint32_t sz>
class fifo_read_iterator
{
  std::array<T, sz>&     data_;
  uint32_t idx;


public:
  explicit fifo_read_iterator(std::array<T, sz>& data,
                              uint32_t index): data_(data), idx(index){}

  bool operator == (fifo_read_iterator<T, sz>& other)
  {
    return this->idx == other.idx;
  }

  bool operator != (fifo_read_iterator<T, sz>& other)
  {
    return !(*this == other);
  }

  T& operator*()
  {
    return data_[idx];
  }

  fifo_read_iterator<T, sz>& operator++()
  {
    ++idx;
    if (idx == sz) {
      idx == 0;
    }
    return *this;
  }

  fifo_read_iterator<T, sz> operator++(int)
  {
    fifo_read_iterator<T, sz> temp = *this;
    ++*this;
    return temp;
  }


};

/// Atomic realization of ring fifo buffer
template<typename T, uint32_t sz>
class fifo_atomic
{
public:
  using value_type [[maybe_unused]] = T;

protected:
  uint32_t              writeIndex = 0;
  uint32_t              readIndex  = 0;
  std::atomic<uint32_t> used       = 0;
  std::array<T, sz>     data;

public:

  /// Get begin read iterator
  fifo_read_iterator<T, sz> begin()
  {
    return fifo_read_iterator<T, sz>(data, readIndex);
  }

  /// Get begin write iterator
  fifo_read_iterator<T, sz> end()
  {
    u_int32_t temp = readIndex + used;
    if (temp >= sz) {
      temp -= sz;
    }
    return fifo_read_iterator(data, temp);
  }

  fifo_atomic() : used(0)
  {
    clear();
  }

  void clear()
  {
    writeIndex = readIndex = 0;
    used.store(0);
  }

  [[nodiscard]] bool empty() const
  {
    return (used.load() == 0);
  }

  [[nodiscard]] bool full() const
  {
    return (used.load() == sz - 1);
  }

  [[nodiscard]] uint32_t size() const
  {
    return (sz - used.load());
  }

  [[nodiscard]] constexpr uint32_t maxsize() const
  {
    return sz;
  }

  bool push(const T &value)
  {
    uint32_t try_used = used.load();
    do {
      if (try_used == sz - 1)
        return false;

    } while (!used.compare_exchange_weak(try_used, try_used + 1));

    data[writeIndex] = value;

    ++writeIndex;
    if (writeIndex == sz)
      writeIndex = 0;

    return true;
  }

  ///
  /// \brief Remove last read element from fifo
  ///
  void pop()
  {
    uint32_t try_used = used.load();
    do {
      if (try_used == 0)
        return;

    } while (!used.compare_exchange_weak(try_used, try_used - 1));

    ++readIndex;
    if (readIndex == sz)
      readIndex = 0;
  }

  std::optional<const T> getOut() const
  {
    return used.load() == 0 ? std::nullopt
                            : std::optional<const T>{ data[readIndex] };
  }

  T& getOutMut() const
  {
    return data[readIndex];
  }

  uint32_t getReadIndex()
  {
    return readIndex;
  }

  uint32_t getWriteIndex()
  {
    return writeIndex;
  }

  uint32_t getUsed()
  {
    return used;
  }

  ///
  /// \brief Get fifo element to write
  /// \return ref to  element
  ///
  T &asyncGetToWrite()
  {
    return data[writeIndex];
  }

  ///
  /// \brief Write complete
  /// \return ref to new write element
  ///
  std::optional<T>asyncWritten()
  {

    uint32_t try_used = used.load(std::memory_order_seq_cst);
    do {
      if (try_used == sz - 1)
        return {};

    } while (!used.compare_exchange_weak(try_used,
                                         try_used + 1));

    ++writeIndex;
    if (writeIndex == sz) {
      writeIndex = 0;
    }

    return std::optional<T>{ data[writeIndex] };
  }
};
} // namespace tl
#endif // FIFO_ATOMIC_H
