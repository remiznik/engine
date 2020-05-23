
#include "gtest/gtest.h"
#include "script_system/vm/chunk.h"

TEST(check_chunk, vm) 
{
    script_system::vm::Chunk chunk;
    script_system::vm::initChunk(&chunk);
    EXPECT_EQ (chunk.count,  0);
    EXPECT_EQ (chunk.capacity,  0);

    script_system::vm::writeChunk(&chunk, script_system::vm::OpCode::OP_RETURN);
    EXPECT_EQ (chunk.count,  1);
    EXPECT_EQ (chunk.capacity,  8);

    script_system::vm::freeChunk(&chunk);
    EXPECT_EQ (chunk.count,  0);
    EXPECT_EQ (chunk.capacity,  0);
} 
    