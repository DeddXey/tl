#include "gtest/gtest.h"
#include "statcnt.h"


//----------------------------------------------------
TEST(statcnt, SlipStatCnt)
{
    float a[] = {
        1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f
    };

    SlipStatCnt<float, 7> sc;

    for (auto i : a) {
        sc.addValue(i);
    }

    EXPECT_FLOAT_EQ(1.0f, sc.getMin());
    EXPECT_FLOAT_EQ(7.0f, sc.getMax());
    EXPECT_FLOAT_EQ(4.0f, sc.getAvg());
}
