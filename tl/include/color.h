#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>
#include <cmath>


struct Color {

    static uint32_t hsvToRgb(float h, float s, float v)
    {
        uint8_t hi = static_cast<uint8_t>(std::floor(h / 60.0f)) % 6;
        float f = (h / 60.0f) - std::floor(h / 60.0f);
        float p = v * (1.0f - s);
        float q = v * (1.0f - (f * s));
        float t = v * (1.0f - ((1.0f - f) * s));

        uint8_t v8= static_cast<uint8_t>(v * 255.0f);
        uint8_t t8= static_cast<uint8_t>(t * 255.0f);
        uint8_t p8= static_cast<uint8_t>(p * 255.0f);
        uint8_t q8= static_cast<uint8_t>(q * 255.0f);

        switch (hi) {
        case 0:
            return static_cast<uint32_t>((v8 << 16) | (t8 << 8) | p8);
        case 1:
            return static_cast<uint32_t>((q8 << 16) | (v8 << 8) | p8);
        case 2:
            return static_cast<uint32_t>((p8 << 16) | (v8 << 8) | t8);
        case 3:
            return static_cast<uint32_t>((p8 << 16) | (q8 << 8) | v8);
        case 4:
            return static_cast<uint32_t>((t8 << 16) | (p8 << 8) | v8);
        case 5:
            return static_cast<uint32_t>((v8 << 16) | (p8 << 8) | q8);
        }

        return 0;
    }


};



#endif // COLOR_H
