
#include "gtest/gtest.h"
#include "script_system/vm/chunk.h"
#include "script_system/vm/vm_debug.h"

TEST(in_chunk, vm) 
{
    script_system::vm::Chunk chunk;
    script_system::vm::initChunk(&chunk);
    EXPECT_EQ (chunk.count,  0);
    EXPECT_EQ (chunk.capacity,  0);
    EXPECT_EQ (chunk.lines, nullptr);
    EXPECT_EQ (chunk.constants.capacity,  0);
    EXPECT_EQ (chunk.constants.count,  0);

    script_system::vm::writeChunk(&chunk, script_system::vm::OpCode::OP_RETURN, 1);
    EXPECT_EQ (chunk.count,  1);
    EXPECT_EQ (chunk.capacity,  8);
    EXPECT_NE (chunk.lines, nullptr);

    script_system::vm::freeChunk(&chunk);
    EXPECT_EQ (chunk.count,  0);
    EXPECT_EQ (chunk.capacity,  0);
    EXPECT_EQ (chunk.constants.capacity,  0);
    EXPECT_EQ (chunk.constants.count,  0);
    EXPECT_EQ (chunk.lines, nullptr);

} 



TEST(check_chunk, vm) 
{

    script_system::vm::Chunk chunk;
    script_system::vm::initChunk(&chunk);
    int constant = script_system::vm::addConstant(&chunk, 1.2);
    
    script_system::vm::writeChunk(&chunk, script_system::vm::OP_CONSTANT, 123);
    script_system::vm::writeChunk(&chunk, constant, 123);
    script_system::vm::writeChunk(&chunk, script_system::vm::OP_RETURN, 123);
    script_system::vm::writeConstant(&chunk, 15.2, 123);
    script_system::vm::writeChunk(&chunk, script_system::vm::OP_RETURN, 123);
    script_system::vm::disassembleChunk(&chunk, "test");

    EXPECT_EQ (chunk.count,  7);
    EXPECT_EQ (chunk.capacity,  8);
    EXPECT_EQ (chunk.constants.capacity,  8);
    EXPECT_EQ (chunk.constants.count,  2);

    script_system::vm::freeChunk(&chunk); 
} 

    