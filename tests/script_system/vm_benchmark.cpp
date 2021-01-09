
#include "gtest/gtest.h"

#include "script_system/vm/vm.h"

namespace {
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
    