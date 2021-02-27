#ifndef LOGLEVELS_H
#define LOGLEVELS_H


#include <cstdint>
enum class LogLevel :uint8_t
{
    none = 0,
    always = 1,
    error = 2,
    warning = 3,
    info = 4,
    debug = 5
};


#endif // LOGLEVELS_H
