#ifndef FIFO_ATOMIC_H
#define FIFO_ATOMIC_H

#include <array>
#include <atomic>
#include <mutex>

template<typename T, uint32_t sz>
class FifoAtomic
{

  struct State
  {
    uint32_t writeIndex = 0;
    uint32_t readIndex  = 0;
    uint32_t used       = 0;
  };

  std::atomic<uint32_t> used;
  std::atomic<uint32_t> readIndex;
  std::atomic<uint32_t> writeIndex;
  std::mutex            writeMutex;

  std::array<T, sz> data;

public:
  FifoAtomic() {}

  //------------------------------------------------------------------------
  void clear()
  {
    used.store(0, std::memory_order_release);
    readIndex.store(0, std::memory_order_release);
    writeIndex.store(0, std::memory_order_release);
  }

  //------------------------------------------------------------------------
  bool empty() const
  {
    return (used.load(std::memory_order_acquire) == 0);
  }

  //------------------------------------------------------------------------
  bool full() const
  {
    return (used.load(std::memory_order_acquire) == sz);
  }

  //------------------------------------------------------------------------
  uint32_t size() const
  {
    return (sz - used.load(std::memory_order_acquire));
  }

  //------------------------------------------------------------------------
  constexpr uint32_t maxsize() const
  {
    return sz;
  }

  //------------------------------------------------------------------------
  bool push(const T /*&*/ value)
  {

    uint32_t us = used.load(std::memory_order_relaxed);

    if (used < sz) {

      uint32_t wr = writeIndex.fetch_add(1, std::memory_order_relaxed);

      auto res = used.compare_exchange_strong(us,
                                              us + 1,
                                              std::memory_order_release,
                                              std::memory_order_relaxed);
    }
    else {
      return false;
    }

    uint32_t wrIn = writeIndex.fetch_add(1, std::memory_order_acquire);

    wrIn = (wrIn + 1) % sz;

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
    //        std::lock_guard<std::mutex> lg(mtx);

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
    //        std::lock_guard lg(mtx);

    if (used != sz - 1) {
      ++used;

      ++writeIndex;
      if (writeIndex == sz)
        writeIndex = 0;
    }

    return data[writeIndex];
  }
};

#endif // FIFO_ATOMIC_H
