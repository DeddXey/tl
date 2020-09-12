#ifndef RANGETEST_H
#define RANGETEST_H

#include "ranges/adc24toint32iterator.h"
#include "ranges/range.h"
#include "ranges/thresholditerator.h"
#include "ranges/transforms.h"
#include <cstdint>


uint8_t inputBuffer[3 * 9] {
    0x00, 0x00, 0x01,
    0x00, 0x00, 0x10,
    0x00, 0x01, 0x00,
    0x00, 0x10, 0x00,
    0x01, 0x00, 0x00,
    0x00, 0x10, 0x00,
    0x00, 0x01, 0x00,
    0x00, 0x00, 0x10,
    0x00, 0x00, 0x01,

};

void testFun()
{

    makeRange(inputBuffer) |
            adc24ToInt32() |
            threshold(50000) |
            printRange();

}


#endif // RANGETEST_H
