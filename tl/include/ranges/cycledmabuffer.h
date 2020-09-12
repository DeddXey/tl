#ifndef CYCLEDMABUFFER_H
#define CYCLEDMABUFFER_H

#include <cstdint>
#include <iterator>




//============================================================================
// Cycle Buffer
//============================================================================
class CycleDmaIteratorSentinel {};


//============================================================================
template <typename T, size_t chunkSize>
class CycleDmaIterator {


    T* const bufPtr;
    const size_t  len;
    const uint32_t chunkCount;

    uint32_t    dmaChunkNumber;
    uint16_t    *dmaPtr;

    uint16_t    *ptr;
    int32_t     off;


    //------------------------------------------------------------------------
    uint32_t getIteratorChunk() const
    {
        return (ptr - bufPtr) / chunkSize;
    }


public:

    //------------------------------------------------------------------------
    CycleDmaIterator(T buffer, size_t length) :
        bufPtr(buffer),
        len(length),
        chunkCount(len / chunkSize),
        dmaChunkNumber(0),
        dmaPtr(buffer),
        ptr(buffer)  {}

    //------------------------------------------------------------------------
    uint16_t* getNextDmaBuffer()
    {
        return dmaPtr;
    }

    //------------------------------------------------------------------------
    void nextDmaChunk()
    {
        ++dmaChunkNumber;
        if (dmaChunkNumber == chunkCount)
            dmaChunkNumber = 0;
        dmaPtr = bufPtr + dmaChunkNumber * chunkSize;
    }

    //------------------------------------------------------------------------
    CycleDmaIterator& operator++()
    {
        ++ptr;
        if (ptr >= bufPtr + len)
            ptr = bufPtr;

        return *this;
    }

    //------------------------------------------------------------------------
    CycleDmaIterator operator++(int)
    {
        CycleDmaIterator tmp(*this);
        operator++();
        return tmp;
    }

    //------------------------------------------------------------------------
    int32_t operator*() const
    {
        return static_cast<int16_t>(*ptr) + off;
    }

    //------------------------------------------------------------------------
    bool operator != (const CycleDmaIteratorSentinel) const
    {
        return (getIteratorChunk() != dmaChunkNumber) ;
    }

    //------------------------------------------------------------------------
    bool operator == (const CycleDmaIteratorSentinel& other) const
    {
        return (!(*this != other));
    }

};

//============================================================================
// Range class
template <typename T, size_t chunkSize, size_t chunkCount>
class CycleDmaRange {

    CycleDmaIterator<T, chunkSize> it1;
    CycleDmaIteratorSentinel       it2;

public:
    template<typename R>
    CycleDmaRange(R& r) :
        it1(std::begin(r), std::end(r) - std::begin(r))
    {}

    //------------------------------------------------------------------------
    CycleDmaIterator<T, chunkSize>  begin() const
    {
        return it1;
    }

    //------------------------------------------------------------------------
    CycleDmaIteratorSentinel  end() const
    {
        return it2;
    }

};


#endif // CYCLEDMABUFFER_H
