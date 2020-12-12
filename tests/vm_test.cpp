
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


class TestVM
{
public:
    TestVM()
    {
        script_system::vm::initVM();
    }
    ~TestVM()
    {
        script_system::vm::freeVM();
    }
};

TEST(vm, two_plus_three) 
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("1 + 2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);    
}

TEST(vm, simple_check) 
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("(-1 + 2) * 3 - -4;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, type_of_values)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("!(5 - 4 > 3 * 2 == !nil);");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, value_string)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("\"str\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}
    

TEST(vm, string_concatenate)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("\"one\" + \"tow\" + \"three\";");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("var a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var_and_initialize)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("var a = 1+2;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, create_var_and_initialize_print)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("var a = 1+2;\n print a;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, create_var_and_assigment)
{
    TestVM vm;
    auto reslut = script_system::vm::interpret("var a = 1+2;\n a = 25;");
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, local_var)
{
    TestVM vm;
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
    TestVM vm;
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
    TestVM vm;
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
    TestVM vm;
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
    TestVM vm;
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
    TestVM vm;
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
    TestVM vm;
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

TEST(vm, for_without_increment_statement)
{
    TestVM vm;
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
    TestVM vm;
    const char* source =
        "{"
        "   var i = 0;"
        "   for(; i < 1; i = i + 1)"
        "   {"
        "       print i;"
        "   }"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, for_without_increment_and_declaration_statement)
{
    TestVM vm;
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

TEST(vm, simple_sum_function)
{
    TestVM vm;
    const char* source =
        "fun sum()"
        "{"
        "   return 1 + 2;"
        "}"
        "print sum();";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}
TEST(vm, function_with_args)
{
    TestVM vm;
    const char* source =
        "fun sum(a, b)"
        "{"
        "   return a + b;"
        "}"
        "sum(1, 3);";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, function_fibonachi_ten)
{
    TestVM vm;
    const char* source =
        "fun fib(a)"
        "{"
        "   if ( a < 2 ) return a;"
        "   return fib(a -1) + fib(a -2);"        
        "}"
        "print fib(10);";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, native_clock_function)
{
    TestVM vm;
    const char* source =
        "var t = clock();"        
        "print t;";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, count_time_calculate_fibonachi_35)
{
    TestVM vm;
    const char* source =
        "var start = clock();"
        "fun fib(a)"
        "{"
        "   if ( a < 2 ) return a;"
        "   return fib(a -1) + fib(a -2);"
        "}"
        "fib(35);"
        "print clock() - start;";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, closure)
{
    TestVM vm;
    //const char* source =
    //    "var x = \"global\";"
    //    "fun outer()"
    //    "{"
    //    " var x = \"outer\";"
    //    " fun inner()"
    //    " {"
    //    "   print x;"
    //    " }"
    //    " inner();"
    //    "}"
    //    "outer();"
    //    ;

    const char* source =
        "fun makeClosure(value){"
        " fun closure() {"
        "   print value;"
        " }"
        " return closure;"
        "}"
        "var doughunt = makeClosure(\"doughunt\");"
        "var bagle = makeClosure(\"bagle\");"
        "doughunt();"
        "bagle();";

    //auto reslut = script_system::vm::interpret(source);
    //EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


// TODO : Fix switch statement
//TEST(vm, switch_statment)
//{
//    const char* source =
//        "{"
//        "   var i = 3;"
//        "   switch (i)"
//        "   {"
//        "       case 1:"
//        "       {"
//        "           print 4;"
//        "       }"
//        "       case 2:"
//        "       {"
//        "           print 5;"  
//        "       }"
//        "       case 3:"
//        "       {"
//        "           print 6;"
//        "       }"      
//        "   }"
//        "}";
//
//    auto result = script_system::vm::interpret(source);
//    EXPECT_EQ(result, script_system::vm::INTERPRET_OK);
//}