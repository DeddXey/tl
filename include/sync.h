#ifndef SYNC_H
#define SYNC_H

#include <atomic>
#include <mutex>
#include "instructions.h"


namespace tl {
// TODO: make usable
class spinlock_mutex
{
  std::atomic_flag flag{ ATOMIC_FLAG_INIT };

public:
  void lock()
  {
    while (flag.test_and_set(std::memory_order_acquire))
      ;
  }

  void unlock()
  {
    flag.clear(std::memory_order_release);
  }
};

class critical_section {
public:
  critical_section()
  {
    cpu::cpsid();
  }

  ~critical_section()
  {
    cpu::cpsie();
  }
};
using lock_guard = std::lock_guard<spinlock_mutex>;



template<typename T = uint8_t>
class SyncCounter
{

  std::atomic<T> value;

public:
  SyncCounter() : value{ 0 } {}

  //------------------------------------------------------------------------
  T get()
  {
    return value.load();
  }

  //------------------------------------------------------------------------
  void set(T val)
  {
    value.store(val);
  }

  //------------------------------------------------------------------------
  auto operator++()
  {
    return value++;
  }

  //------------------------------------------------------------------------
  void incMax(T max)
  {
    T oldValue;
    do {
      oldValue = value.load(std::memory_order_relaxed);
      if (oldValue == max)
        break;
    } while (!value.compare_exchange_weak(oldValue,
                                          oldValue + 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
  }

  //------------------------------------------------------------------------
  auto &operator--()
  {
    return value--;
  }

  //------------------------------------------------------------------------
  void decMin(T min)
  {
    T oldValue;
    do {
      oldValue = value.load(std::memory_order_relaxed);
      if (oldValue == min)
        break;
    } while (!value.compare_exchange_weak(oldValue,
                                          oldValue - 1,
                                          std::memory_order_release,
                                          std::memory_order_relaxed));
  }

};
} // namespace tl

#endif // SYNC_H
