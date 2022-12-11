#ifndef TLTIME_H
#define TLTIME_H

#include <cstdint>

namespace tl {

struct Time {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} __attribute__((aligned(4)));

}


#endif // TLTIME_H
