#ifndef CYCLEBUFFER_H
#define CYCLEBUFFER_H

#include "sync.h"
#include <cstdint>

///
/// Потокобезопасный циклический пул буферов
///
template<uint32_t size, uint8_t num, typename T = uint8_t>
class CycleBuffer
{

  T                  data[num][size];
  uint8_t            writeIndex = 0;
  uint8_t            readIndex  = 0;
  tl::spinlock_mutex mutex;

  tl::SyncCounter<uint8_t> used;

public:
  CycleBuffer() = default;

  void resetAndWrite()
  {
    tl::lock_guard lock_guard(mutex);
    writeIndex = 0;
    readIndex  = 0;
    used.set(0);
  }

  T *currentWrite()
  {
    return data[0];
  }

  T *nextWrite()
  {
    tl::lock_guard lock_guard(mutex);
    if (used.get() == num) {
      return nullptr;
    }

    uint8_t tmpIndex = writeIndex;

    used.decMin(0);

    ++writeIndex;
    if (writeIndex == num) {
      writeIndex = 0;
    }

    return data[tmpIndex];
  }

  T *nextRead()
  {
    tl::lock_guard lock_guard(mutex);
    if (used.get() == 0) {
      return nullptr;
    }

    uint8_t tmpIndex = readIndex;

    used.decMin(0);

    ++readIndex;
    if (readIndex == num) {
      readIndex = 0;
    }

    return data[tmpIndex];
  }
};

///
/// Потокобезопасный циклический пул всего
///
template<uint8_t num, typename T = uint8_t>
class Cycler
{

  T       data[num];
  uint8_t writeIndex = 0;
  uint8_t readIndex  = 0;

  tl::SyncCounter<int> used;
  tl::spinlock_mutex   mutex;

public:
  Cycler() = default;

  void reset()
  {
    tl::lock_guard lock_guard(mutex);
    writeIndex = 0;
    readIndex  = 0;
    used.set(0);
  }

  T *currentWrite()
  {
    tl::lock_guard lock_guard(mutex);
    return data[writeIndex];
  }

  T *currentRead()
  {
    tl::lock_guard lock_guard(mutex);
    return data[readIndex];
  }

  bool isEmpty()
  {
    tl::lock_guard lock_guard(mutex);
    return (used.get() == 0);
  }

  T *nextWrite()
  {
    tl::lock_guard lock_guard(mutex);
    if (used.get() == num) {
      return nullptr;
    }

    uint8_t tmpIndex = writeIndex;

    used.incMax(num);

    ++writeIndex;
    if (writeIndex == num) {
      writeIndex = 0;
    }

    return &data[tmpIndex];
  }

  T *nextRead()
  {
    tl::lock_guard lock_guard(mutex);
    if (used.get() == 0) {
      return nullptr;
    }

    uint8_t tmpIndex = readIndex;

    used.decMin(0);

    ++readIndex;
    if (readIndex == num) {
      readIndex = 0;
    }

    return &data[tmpIndex];
  }
};

#endif // CYCLEBUFFER_H
