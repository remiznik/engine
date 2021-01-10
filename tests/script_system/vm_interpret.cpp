
#include "gtest/gtest.h"
#include "script_system/script.h"
#include "script_system/native_function.h"

namespace {
    class TestScript
    {
    public:
        TestScript()
        {
            script.init();
        }
        ~TestScript()
        {
            script.fini();
        }
        
        script_system::Script script;

    };
}

TEST(vm, two_plus_three)
{
    TestScript vm;
    auto reslut = vm.script.run("1 + 2;");    
    EXPECT_TRUE(reslut);
}

TEST(vm, simple_check)
{
    TestScript vm;
    auto reslut = vm.script.run("(-1 + 2) * 3 - -4;");
    EXPECT_TRUE(reslut);
}

TEST(vm, type_of_values)
{
    TestScript vm;
    auto reslut = vm.script.run("!(5 - 4 > 3 * 2 == !nil);");
    EXPECT_TRUE(reslut);
}

TEST(vm, value_string)
{
    TestScript vm;
    auto reslut = vm.script.run("\"str\";");
    EXPECT_TRUE(reslut);
}


TEST(vm, string_concatenate)
{
    TestScript vm;
    auto reslut = vm.script.run("\"one\" + \"tow\" + \"three\";");
    EXPECT_TRUE(reslut);
}

TEST(vm, create_var)
{
    TestScript vm;
    auto reslut = vm.script.run("var a;");
    EXPECT_TRUE(reslut);
}

TEST(vm, create_var_and_initialize)
{
    TestScript vm;
    auto reslut = vm.script.run("var a = 1+2;");
    EXPECT_TRUE(reslut);
}

TEST(vm, create_var_and_initialize_print)
{
    TestScript vm;
    auto reslut = vm.script.run("var a = 1+2;\n print a;");
    EXPECT_TRUE(reslut);
}


TEST(vm, create_var_and_assigment)
{
    TestScript vm;
    auto reslut = vm.script.run("var a = 1+2;\n a = 25;");
    EXPECT_TRUE(reslut);
}


TEST(vm, local_var)
{
    TestScript vm;
    const char* source =
        "{"
        "   var a = 1+2;"
        "   {"
        "       var b = a + 3;"
        "       a = b + 3;"
        "   }"
        "   print a;"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, if_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   var a = 1+2;"
        "   if (a > 0)"
        "   {"
        "    print a;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, if_else_statement)
{
    TestScript vm;
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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, and_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   print true and true;" // true        
        "   print true and false;" // false
        "   print false and false;" // false
        "   print false and true;" // false
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, or_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   print true or true;" // true        
        "   print true or false;" // true
        "   print false or false ;" // false
        "   print false or true;" // true
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, while_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   var i = 0;"
        "   while(i < 10)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, for_full_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   for(var i =0; i < 10; i = i + 1)"
        "   {"
        "       print i;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, for_without_increment_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   for(var i =0; i < 10;)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, for_without_declaretion_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   var i = 0;"
        "   for(; i < 1; i = i + 1)"
        "   {"
        "       print i;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}


TEST(vm, for_without_increment_and_declaration_statement)
{
    TestScript vm;
    const char* source =
        "{"
        "   var i = 0;"
        "   for(; i < 10;)"
        "   {"
        "       print i;"
        "       i = i + 1;"
        "   }"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, simple_sum_function)
{
    TestScript vm;
    const char* source =
        "fun sum()"
        "{"
        "   return 1 + 2;"
        "}"
        "print sum();";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}
TEST(vm, function_with_args)
{
    TestScript vm;
    const char* source =
        "fun sum(a, b)"
        "{"
        "   return a + b;"
        "}"
        "sum(1, 3);";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, function_fibonachi_ten)
{
    TestScript vm;
    const char* source =
        "fun fib(a)"
        "{"
        "   if ( a < 2 ) return a;"
        "   return fib(a -1) + fib(a -2);"
        "}"
        "print fib(10);";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}


TEST(vm, closure)
{
    TestScript vm;
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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, closure_outside)
{
    TestScript vm;

    const char* source =
        "fun outer(){"
        "  var x = \"outside\";"
        "  fun inner() {"
        "     print x;"
        "  }"
        "  inner();"
        "}"
        "outer();";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, closure_closeove)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}


TEST(vm, gc_test_one)
{
    TestScript vm;

    const char* source =
        "var a = \"first value\";"
        "a = \"update\";"
        "print a;";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, gc_test_two)
{
    TestScript vm;

    const char* source =
        "var global = \"string\";"
        "{"
        "  var local = \"another\";"
        "  print global + local;"
        "}";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_define)
{
    TestScript vm;

    const char* source =
        "class Brioche{}"
        "print Brioche;";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_instance)
{
    TestScript vm;

    const char* source =
        "class Brioche{}"
        "print Brioche();";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}


TEST(vm, class_instance_fields)
{
    TestScript vm;

    const char* source =
        "class Pair{}"
        "var pair = Pair();"
        "pair.first = 1;"
        "pair.second = 2;"
        "print pair.first + pair.second;";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_method)
{
    TestScript vm;

    const char* source =
        "class Scone{"
        "   topping(first, second) {"
        "       print \"second with \" + first + \" and \" + second;"
        "   }"
        "}"
        "var scone = Scone();"
        "scone.topping(\"berries\", \"cream\");"
        ;

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_init_method)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_invoking_fields)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm, class_supperclass)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
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
//    auto result = vm.script.run(source);
//    EXPECT_EQ(result, script_system::vm::INTERPRET_OK);
//}
