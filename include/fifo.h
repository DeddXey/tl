#ifndef FIFO_H
#define FIFO_H

//#include "dbg.h"
#include "sync.h"
#include <array>
#include <mutex>

namespace tl {
template<typename T, uint32_t sz>
class Fifo
{

  uint32_t             writeIndex = 0;
  uint32_t             readIndex  = 0;
  uint32_t             used       = 0;
  tl::critical_section mtx;

public:
  std::array<T, sz> data;

  Fifo() : writeIndex(0), readIndex(0), used(0)
  {
    clear();
  }

  //------------------------------------------------------------------------
  void clear()
  {
    tl::critical_section cs;
    writeIndex = readIndex = 0;
    used                   = 0;
  }

  //------------------------------------------------------------------------
  bool empty() const
  {
    return (used == 0);
  }

  //------------------------------------------------------------------------
  bool full() const
  {
    return (used == sz);
  }

  //------------------------------------------------------------------------
  uint32_t size() const
  {
    return (sz - used);
  }

  //------------------------------------------------------------------------
  constexpr uint32_t maxsize() const
  {
    return sz;
  }

  //------------------------------------------------------------------------
  bool push(const T /*&*/ value)
  {
    tl::critical_section cs;

    bool out = true;
    if (used != sz) {

      data[writeIndex] = value;
      ++used;

      ++writeIndex;
      if (writeIndex == sz)
        writeIndex = 0;
    }
    else {
      out = false;
    }

    return out;
  }

  //------------------------------------------------------------------------
  ///
  /// \brief Remove last read element from fifo
  ///
  void pop()
  {
    tl::critical_section cs;

    if (used != 0) {
      --used;

      ++readIndex;
      if (readIndex == sz)
        readIndex = 0;
    }
  }

  //------------------------------------------------------------------------
  const T &getOut() const
  {
    return data[readIndex];
  }

  //------------------------------------------------------------------------
  uint32_t getReadIndex()
  {
    return readIndex;
  }

  //------------------------------------------------------------------------
  uint32_t getWriteIndex()
  {
    return writeIndex;
  }

  //------------------------------------------------------------------------
  uint32_t getUsed()
  {
    return used;
  }

  //------------------------------------------------------------------------
  ///
  /// \brief Get fifo element to write
  /// \return ref to  element
  ///
  T &asyncGetToWrite()
  {
    return data[writeIndex];
  }

  //------------------------------------------------------------------------
  ///
  /// \brief Write complete
  /// \return ref to new write element
  ///
  T &asyncWritten()
  {
    tl::critical_section cs;

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
