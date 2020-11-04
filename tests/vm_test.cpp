
#include "gtest/gtest.h"
#include "script_system/vm/chunk.h"
#include "script_system/vm/debug.h"
#include "script_system/vm/vm.h"

TEST(vm, in_chunk) 
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


TEST(vm, two_plus_three) 
{
    auto reslut = script_system::vm::interpret("1 + 2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, simple_check) 
{
    auto reslut = script_system::vm::interpret("(-1 + 2) * 3 - -4;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, type_of_values)
{
    auto reslut = script_system::vm::interpret("!(5 - 4 > 3 * 2 == !nil);");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, value_string)
{
    auto reslut = script_system::vm::interpret("\"str\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}
    

TEST(vm, string_concatenate)
{
    auto reslut = script_system::vm::interpret("\"one\" + \"tow\" + \"three\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var)
{
    auto reslut = script_system::vm::interpret("var a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var_and_initialize)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var_and_initialize_print)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;\n print a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, create_var_and_assigment)
{
    auto reslut = script_system::vm::interpret("var a = 1+2;\n a = 25;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, local_var)
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

TEST(vm, if_statement)
{
    const char* source =
        "{"
        "   var a = 1+2;"
        "   if (a > 0)"
        "   {"
        "    print a;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, if_else_statement)
{
    const char* source =
        "{"
        "   var a = 1+2;"
        "   if (a < 0)"
        "   {"
        "    print a;"
        "   }"
        "   else"
        "   {"
        "     print false;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, and_statement)
{
    const char* source =
        "{"
        "   print true and true;" // true        
        "   print true and false;" // false
        "   print false and false;" // false
        "   print false and true;" // false
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, or_statement)
{
    const char* source =
        "{"
        "   print true or true;" // true        
        "   print true or false;" // true
        "   print false or false ;" // false
        "   print false or true;" // true
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, while_statement)
{
    const char* source =
        "{"
        "   var i = 0;"
        "   while(i < 10)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, for_full_statement)
{
    const char* source =
        "{"
        "   for(var i =0; i < 10; i = i + 1)"
        "   {"
        "       print i;"        
        "   }"
        "}";
     
    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, for_withot_increment_statement)
{
    const char* source =
        "{"
        "   for(var i =0; i < 10;)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, for_without_declaretion_statement)
{
    const char* source =
        "{"
        "   var i = 0;"
        "   for(; i < 10; i = i + 1)"
        "   {"
        "       print i;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, for_without_increment_and_declaration_statement)
{
    const char* source =
        "{"
        "   var i = 0;"
        "   for(; i < 10;)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, switch_statment)
{
    const char* source =
        "{"
        "   var i = 3;"
        "   switch (i)"
        "   {"
        "       case 1:"
        "       {"
        "           print 1;"
        "       }"
        "       case 2:"
        "       {"
        "           print 2;"  
        "       }"      
        "   }"
        "}";

    auto result = script_system::vm::interpret(source);
    EXPECT_EQ(result, script_system::vm::INTERPRET_OK);
}