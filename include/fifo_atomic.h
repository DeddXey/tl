#ifndef FIFO_H
#define FIFO_H

//#include "dbg.h"
#include <array>
#include <optional>
#include <atomic>
#include <mutex>

namespace tl {
template<typename T, uint32_t sz>
class fifo_atomic
{

  uint32_t              writeIndex = 0;
  uint32_t              readIndex  = 0;
  std::atomic<uint32_t> used       = 0;

public:
  std::array<T, sz> data;

  fifo_atomic() : writeIndex(0), readIndex(0), used(0)
  {
    clear();
  }

  void clear()
  {
    //    tl::critical_section cs;
    writeIndex = readIndex = 0;
    used.store(0);
  }

  bool empty() const
  {
    return (used.load() == 0);
  }

  bool full() const
  {
    return (used.load() == sz);
  }

  uint32_t size() const
  {
    return (sz - used.load());
  }

  constexpr uint32_t maxsize() const
  {
    return sz;
  }

  bool push(const T &value)
  {
    uint32_t try_used = used.load();
    do {
      if (try_used == sz)
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

  std::optional<const T>getOut() const
  {
    return used.load()==0?std::nullopt:std::optional<const T>{data[readIndex]};
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
  T &asyncWritten()
  {

    if (used != sz - 1) {
      ++used;

      ++writeIndex;
      if (writeIndex == sz)
        writeIndex = 0;
    }

    return data[writeIndex];
  }
};
} // namespace tl
#endif // FIFO_H
