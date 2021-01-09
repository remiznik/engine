
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
//
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

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, closure_outside)
{
    TestVM vm;
    
    const char* source =
        "fun outer(){"
        "  var x = \"outside\";"
        "  fun inner() {"
        "     print x;"
        "  }"
        "  inner();"
        "}"
        "outer();";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, closure_closeove)
{
    TestVM vm;

    const char* source =
        "var globalSet;"
        "var globalGet;"        
        "fun main()"
        "{"
        "  var a = \"initial\";"
        "  fun set() { a = \"updated\"; }"
        "  fun get() { print a; }"
        "  globalSet = set;"
        "  globalGet = get;"
        "}"
        "main();"
        "globalSet();"
        "globalGet();"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, gc_test_one)
{
    TestVM vm;

    const char* source =
        "var a = \"first value\";"
        "a = \"update\";"
        "print a;";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, gc_test_two)
{
    TestVM vm;

    const char* source =
        "var global = \"string\";"
        "{"
        "  var local = \"another\";"
        "  print global + local;"
        "}";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_define)
{
    TestVM vm;

    const char* source =
        "class Brioche{}"
        "print Brioche;";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_instance)
{
    TestVM vm;

    const char* source =
        "class Brioche{}"
        "print Brioche();";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}


TEST(vm, class_instance_fields)
{
    TestVM vm;

    const char* source =
        "class Pair{}"
        "var pair = Pair();"
        "pair.first = 1;"
        "pair.second = 2;"
        "print pair.first + pair.second;";

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_method)
{
    TestVM vm;

    const char* source =
        "class Scone{"
        "   topping(first, second) {"
        "       print \"second with \" + first + \" and \" + second;"
        "   }"
        "}"
        "var scone = Scone();"
        "scone.topping(\"berries\", \"cream\");"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_init_method)
{
    TestVM vm;

    const char* source =
        "class CoffeMaker{"
        "   init(coffee) {"
        "    this.coffee = coffee;"
        "   }"
        "   brew() {"
        "     print \"Enjoy your cup of \" + this.coffee;"
        "     this.coffee = nil;"
        "   }"
        "}"
        "var maker = CoffeMaker(\"coffe adn chicory\");"
        "maker.brew();"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_invoking_fields)
{
    TestVM vm;

    const char* source =
        "class Oops{"
        "   init() {"
        "       fun f() {"
        "           print \"not a method\";"  
        "       }"
        "       this.field = f;"
        "   }"        
        "}"
        "var oops = Oops();"
        "oops.field();"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_supperclass)
{
    TestVM vm;

    const char* source =
        "class Doughnut {"
        "   cook() {"
        "       print \"Dunk in the fryer.\";"
        "       this.finish(\"sprinles\");"
        "   }"
        "   finish(ingredient) {"
        "       print \"Finish with \" + ingredient;"
        "   }"
        "}"
        "class Cruller < Doughnut {"
        "   finish(ingredient) {"
        "       super.finish(\"icnig\");"
        "   }"
        "}"
        "var cruller = Cruller();"
        "cruller.cook();"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_benchmark_time)
{
    TestVM vm;

    const char* source =
        "class Zoo{"
        "   init() {"
        "       this.aardvark = 1;"
        "       this.baboon = 1;"
        "       this.cat  = 1;"
        "       this.donkey = 1;"
        "       this.elephant = 1;"
        "       this.fox = 1;"
        "   }"
        "   ant() { return this.aardvark; }"
        "   banana() { return this.baboon; }"
        "   tuna() { return this.cat; }"
        "   hay() { return this.donkey; }"
        "   grass() { return this.elephant; }"
        "   mouse() { return this.fox; }"
        "}"
        "var zoo = Zoo();"
        "var sum = 0;"
        "var start = clock();"
        "while (sum < 100000000) {"
        "   sum = sum + zoo.ant()"
        "    + zoo.banana()"
        "    + zoo.tuna()"
        "    + zoo.hay()"
        "    + zoo.grass()"
        "    + zoo.mouse();"
        "}"        
        "print clock() - start;"
        "print sum;"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
}

TEST(vm, class_benchmark_count)
{
    TestVM vm;

    const char* source =
        "class Zoo{"
        "   init() {"
        "       this.aardvark = 1;"
        "       this.baboon = 1;"
        "       this.cat  = 1;"
        "       this.donkey = 1;"
        "       this.elephant = 1;"
        "       this.fox = 1;"
        "   }"
        "   ant() { return this.aardvark; }"
        "   banana() { return this.baboon; }"
        "   tuna() { return this.cat; }"
        "   hay() { return this.donkey; }"
        "   grass() { return this.elephant; }"
        "   mouse() { return this.fox; }"
        "}"
        "var zoo = Zoo();"
        "var count = 0;"
        "var sum = 0;"
        "var start = clock();"
        "while ((clock() - start) < 1) {"
        "   sum = sum + zoo.ant()"
        "    + zoo.banana()"
        "    + zoo.tuna()"
        "    + zoo.hay()"
        "    + zoo.grass()"
        "    + zoo.mouse();"
        "   count = count + 1;"
        "}"        
        "print count;"
        ;

    auto reslut = script_system::vm::interpret(source);
    EXPECT_EQ(reslut, script_system::vm::INTERPRET_OK);
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