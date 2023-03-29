#include "doctest.h"
#include "statcnt.h"
#include "utility.h"

//----------------------------------------------------
TEST_CASE("statcnt, SlipStatCnt")
{
    float a[] = {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f
    };

    SlipStatCnt<float, 7> sc;

    for (auto i : a) {
        sc.addValue(i);
    }

    CHECK(tl::isFloatingSame(1.0f, sc.getMin()));
    CHECK(tl::isFloatingSame(7.0f, sc.getMax()));
    CHECK(tl::isFloatingSame(4.0f, sc.getAvg()));
}
