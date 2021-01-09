
#include "gtest/gtest.h"
#include "script_system/vm/utils.h"

TEST(conver_int_to_uint, vm) 
{

    for (int i = 0; i < 10000; i++)
    {
        uint8_t a , b;
        script_system::vm::utils::convertIntTwoUint(i, a, b);
        int r = 0;
        script_system::vm::utils::convertTwoUintInt(a, b, r);

        EXPECT_EQ(r, i);
    }

} 

    