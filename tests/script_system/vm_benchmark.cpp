
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
            script.registreFunction("clock", makeShared<script_system::ClockFunction>());
        }
        ~TestScript()
        {
            script.fini();
        }

        script_system::Script script;

    };
}


TEST(vm_benchmark, class_time)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm_benchmark, class_count)
{
    TestScript vm;

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

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm_benchmark, native_clock_function)
{
    TestScript vm;
    const char* source =
        "var t = clock();"
        "print t;";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}

TEST(vm_benchmark, count_time_calculate_fibonachi_35)
{
    TestScript vm;
    const char* source =
        "var start = clock();"
        "fun fib(a)"
        "{"
        "   if ( a < 2 ) return a;"
        "   return fib(a -1) + fib(a -2);"
        "}"
        "fib(35);"
        "print clock() - start;";

    auto reslut = vm.script.run(source);
    EXPECT_TRUE(reslut);
}
    