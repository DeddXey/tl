#ifndef FIFO_ATOMIC_H
#define FIFO_ATOMIC_H

#include <array>
#include <atomic>
#include <mutex>


template <typename T, uint32_t sz>
class FifoAtomic {

    uint32_t                writeIndex = 0;
    uint32_t                readIndex = 0;
    std::atomic<uint32_t>   used = 0;
    std::mutex              mtx;


public:
    std::array<T, sz> data;

    FifoAtomic():
        writeIndex(0),readIndex(0),used(0)
    {
        clear();
    }

    //------------------------------------------------------------------------
    void clear()
    {
        std::lock_guard<std::mutex> lg(mtx);
        writeIndex = readIndex = 0;
        used = 0;

    }

    //------------------------------------------------------------------------
    bool empty() const
    {
        return (used.load() == 0);
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
    bool push(const T/*&*/ value)
    {
        std::lock_guard<std::mutex> lg(mtx);

        bool out = true;
        if (used.load() != sz) {

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
        std::lock_guard<std::mutex> lg(mtx);

        if (used.load() != 0) {
            --used;

            ++readIndex;
            if (readIndex == sz)
                readIndex = 0;
        }
    }

    //------------------------------------------------------------------------
    const T& getOut() const
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
        return used.load();
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Get fifo element to write
    /// \return ref to  element
    ///
    T& asyncGetToWrite()
    {
        return data[writeIndex];
    }

    //------------------------------------------------------------------------
    ///
    /// \brief Write complete
    /// \return ref to new write element
    ///
    T& asyncWritten()
    {
        std::lock_guard lg(mtx);

        if (used != sz-1) {
            ++used;

            ++writeIndex;
            if (writeIndex == sz)
                writeIndex = 0;
        }

        return data[writeIndex];
    }
};



#endif // FIFO_ATOMIC_H
