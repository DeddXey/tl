#include "gtest/gtest.h"

#include "utility.h"

//----------------------------------------------------
TEST(utility, setBits)
{
   auto out =
        tl::setBits(3, 1,
                    1, 3);

   EXPECT_EQ(out, 0xe);

}
