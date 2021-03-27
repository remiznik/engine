
#include "gtest/gtest.h"
#include "render/utils.h"


using namespace render::math;

TEST(compare, math_vector)
{
    EXPECT_EQ(Vector3(1, 1, 1), Vector3(1, 1, 1));
    EXPECT_EQ(Vector3(1, -1, 1), Vector3(1, -1, 1));
    EXPECT_EQ(Vector3(1.0, -1, 1), Vector3(1.0, -1, 1));
}

TEST(sub, math_vector) 
{   
    EXPECT_EQ(Vector3(1, 1, 1) - Vector3(2, 2, 2), Vector3(-1, -1, -1));
    EXPECT_EQ(Vector3(2, 2, 2) - Vector3(1, 1, 1), Vector3(1, 1, 1));
} 

TEST(add, math_vector)
{
    EXPECT_EQ(Vector3(1, 1, 1) + Vector3(2, 2, 2), Vector3(3, 3, 3));
    EXPECT_EQ(Vector3(1, 2, 3) + Vector3(1, 1, 1), Vector3(2, 3, 4));
}


TEST(length, math_vector)
{
    EXPECT_FLOAT_EQ(length(Vector3(1, 1, 1)), static_cast<float>(1.7320508));
    EXPECT_FLOAT_EQ(length(Vector3(2, 2, 2)), static_cast<float>(3.4641016));
}

TEST(normalize, math_vector)
{
    EXPECT_EQ(normalize(Vector3(1, 1, 1)), Vector3(0.577350259, 0.577350259, 0.577350259));
    EXPECT_EQ(normalize(Vector3(1, 2, 3)), Vector3(0.267261237, 0.534522474, 0.801783681));
}

TEST(cross, math_vector)
{
    EXPECT_EQ(cross(Vector3(1, 1, 1) , Vector3(2, 2, 2)), Vector3(0, 0, 0));

    auto t = cross(Vector3(0, 3, 1), Vector3(2, 2, 2));
    EXPECT_EQ(cross(Vector3(0, 3, 1), Vector3(2, 2, 2)), Vector3(4, 2, -6));
}

    