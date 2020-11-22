#ifndef FMT_H
#define FMT_H
#include <cstdint>


template<std::size_t Size>
auto constexpr toInt(const char (&in)[Size])
{
    int out = 0;
    bool is0 = false;

    for(char i : in) {

        if (i == '-') {
            out = -out;
        }
        else {

            if ((is0) &&
            ((i == )))


            if (i == 0) {
                is0 = true;
            }
            else {
                is0 = false;
            }

    }

    return out;
}
#endif // FMT_H
