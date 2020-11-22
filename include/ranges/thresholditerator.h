#ifndef THRESHOLDITERATOR_H
#define THRESHOLDITERATOR_H

#include "ranges/range.h"

#include <cstdint>
#include <utility>

enum class ThresholdType {
    rise,
    fall,
    both,
};


//============================================================================
auto threshold(int32_t threshold,
               ThresholdType thrType = ThresholdType::rise) noexcept
{
    auto ret  = [=](auto ran)
    {
        auto prev = *ran.begin();
        auto it = ran.begin()++;

        for(; it != ran.end(); ++it) {

            bool riseReq = (prev <= threshold) &&
                           (*it >= threshold);

            bool fallReq = (prev >= threshold) &&
                           (*it <= threshold);

            if ((thrType == ThresholdType::rise) &&
                    (riseReq))
                break;

            if ((thrType == ThresholdType::fall) &&
                    (fallReq))
                break;

            if ((thrType == ThresholdType::both) &&
                    ((fallReq) + (riseReq)))
                break;
            };

        return Range<decltype(std::begin(ran)),
                decltype(std::end(ran))>
                    (it, std::end(ran));
    };

    return ret;
}




#endif // THRESHOLDITERATOR_H
