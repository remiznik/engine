
#include "gtest/gtest.h"
#include "script_system/vm/chunk.h"
#include "script_system/vm/debug.h"
#include "script_system/vm/vm.h"

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


TEST(two_plus_three, vm) 
{
    auto reslut = script_system::vm::interpret("1 + 2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(simple_check, vm) 
{
    auto reslut = script_system::vm::interpret("(-1 + 2) * 3 - -4;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(type_of_values, vm) 
{
    auto reslut = script_system::vm::interpret("!(5 - 4 > 3 * 2 == !nil);");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(value_string, vm)
{
    auto reslut = script_system::vm::interpret("\"str\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}
    

TEST(string_concatenate, vm)
{
    auto reslut = script_system::vm::interpret("\"one\" + \"tow\" + \"three\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(create_var, vm)
{
    auto reslut = script_system::vm::interpret("var a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(create_var_and_initialize, vm)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(create_var_and_initialize_print, vm)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;\n print a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(create_var_and_assigment, vm)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;\n a = 25;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(local_var, vm)
{
    const char* source =
        "{"
        "   var a = 1+2;"
        "   {"
        "       var b = a + 3;"
        "       a = b + 3;"
        "   }"
        "   print a;"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}
