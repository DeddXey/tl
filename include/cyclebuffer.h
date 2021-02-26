#ifndef CYCLEBUFFER_H
#define CYCLEBUFFER_H

#include <cstdint>
#include "sync.h"

///
/// Потокобезопасный циклический пул буферов
///
template<uint32_t size, uint8_t num, typename T= uint8_t>
class CycleBuffer {

    T data[num][size];
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;
    tl::spinlock_mutex mutex;

    tl::SyncCounter<uint8_t> used;

public:

    //........................................................................
    CycleBuffer() :
            writeIndex{0},
            readIndex{0} {}

    //........................................................................
    void resetAndWrite() {
        tl::lock_guard lg(mutex);
        writeIndex = 0;
        readIndex = 0;
        used.set(0);
    }

    T *currentWrite() {
        return data[0];
    }

    //........................................................................
    T *nextWrite() {
        tl::lock_guard lg(mutex);
        if (used.get() == num)
            return nullptr;

        uint8_t tmpIndex = writeIndex;

        used.decMin(0);

        ++writeIndex;
        if (writeIndex == num)
            writeIndex = 0;

        return data[tmpIndex];
    }

    //........................................................................
    T *nextRead() {
        tl::lock_guard lg(mutex);
        if (used.get() == 0)
            return nullptr;

        uint8_t tmpIndex = readIndex;

        used.decMin(0);

        ++readIndex;
        if (readIndex == num)
            readIndex = 0;

        return data[tmpIndex];
    }
};

///
/// Потокобезопасный циклический пул всего
///
template<uint8_t num, typename T= uint8_t>
class Cycler {

    T data[num];
    volatile uint8_t writeIndex = 0;
    volatile uint8_t readIndex = 0;

//		std_atomic<uint8_t>		used; //TODO: to atomic
    tl::SyncCounter<int> used;
    tl::spinlock_mutex mutex;

public:

    //........................................................................
    Cycler() :
            writeIndex{0},
            readIndex{0} {}

    //........................................................................
    void reset() {
        tl::lock_guard lg(mutex);
        writeIndex = 0;
        readIndex = 0;
        used.set(0);
    }

    T *currentWrite() {
        tl::lock_guard lg(mutex);
        return data[writeIndex];
    }

    //........................................................................
    T *currentRead() {
        tl::lock_guard lg(mutex);
        return data[readIndex];
    }

    //........................................................................
    bool isEmpty() {
        tl::lock_guard lg(mutex);
        return (used.get() == 0);
    }

    //........................................................................
    T *nextWrite() {
        tl::lock_guard lg(mutex);
        if (used.get() == num)
            return nullptr;

        uint8_t tmpIndex = writeIndex;

        used.incMax(num);

        ++writeIndex;
        if (writeIndex == num)
            writeIndex = 0;

        return &data[tmpIndex];
    }

    //........................................................................
    T *nextRead() {
        tl::lock_guard lg(mutex);
        if (used.get() == 0)
            return nullptr;

        uint8_t tmpIndex = readIndex;

        used.decMin(0);

        ++readIndex;
        if (readIndex == num)
            readIndex = 0;

        return &data[tmpIndex];
    }
};


#endif //CYCLEBUFFER_H
