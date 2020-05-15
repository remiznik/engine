
#include "gtest/gtest.h"
#include "core/value.h"

TEST(first, test) 
{
    
    EXPECT_EQ (core::Value("123"),  core::Value("123"));    
}
