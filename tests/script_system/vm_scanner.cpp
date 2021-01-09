
#include "gtest/gtest.h"
#include "script_system/vm/scanner.h"
#include "script_system/vm/chunk.h"

TEST(get_letter, vm_scanner) 
{
    script_system::vm::initScanner("2");
    script_system::vm::Token token = script_system::vm::scanToken();

    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );
} 

TEST(simple_exr, vm_scanner) 
{
    script_system::vm::initScanner("2+3");
    script_system::vm::Token token;

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_PLUS );

    token = script_system::vm::scanToken();
    EXPECT_EQ (token.type, script_system::vm::TOKEN_NUMBER );
} 

TEST(vm, in_chunk)
{
    script_system::vm::Chunk chunk;
    script_system::vm::initChunk(&chunk);
    EXPECT_EQ(chunk.count, 0);
    EXPECT_EQ(chunk.capacity, 0);
    EXPECT_EQ(chunk.lines, nullptr);
    EXPECT_EQ(chunk.constants.capacity, 0);
    EXPECT_EQ(chunk.constants.count, 0);

    script_system::vm::writeChunk(&chunk, script_system::vm::OpCode::OP_RETURN, 1);
    EXPECT_EQ(chunk.count, 1);
    EXPECT_EQ(chunk.capacity, 8);
    EXPECT_NE(chunk.lines, nullptr);

    script_system::vm::freeChunk(&chunk);
    EXPECT_EQ(chunk.count, 0);
    EXPECT_EQ(chunk.capacity, 0);
    EXPECT_EQ(chunk.constants.capacity, 0);
    EXPECT_EQ(chunk.constants.count, 0);
    EXPECT_EQ(chunk.lines, nullptr);
}