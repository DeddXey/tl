#ifndef OSCILLOSCOPE_H
#define OSCILLOSCOPE_H

#include "log/log.h"
#include <array>

///
/// Auxilary class for adc data output
///
template<typename T, int N, int reduction = 1>
class Oscilloscope {

    using BufferType = std::array<T, N>;

    BufferType buffer;
    typename BufferType::iterator itr = buffer.begin();
    int counter = reduction;

public:
    bool addSample(T sample)
    {
        if (counter == 0) {
            *itr = sample;
            ++itr;

            if (itr == buffer.end()) {
                return true;
            }
            counter = reduction;
        }

        --counter;

        return false;
    }

    void print()
    {
        for (auto i: buffer) {
            con.debug() << Use::dec << i << Use::endl;
        }
    }

    void reset()
    {
        itr = buffer.begin();
    }
};


#endif // OSCILLOSCOPE_H
