
#include "gtest/gtest.h"

#include "script_system/script.h"
#include "script_system/native_function.h"


TEST(script, registr_native_function) 
{
    script_system::Script script;
    script.init();

    class NativeSum : public script_system::NativeFunction
    {
    public:
        script_system::vm::Value call(int, script_system::vm::Value*) override
        {
            return script_system::vm::Value();
        }
    };

    script.registreFunction("nativeSum", makeShared<NativeSum>());

    const char* source =
        "var test = 1;"
        "test = nativeSum(1, 2);"
        "print test;";

    auto result = script.run(source);
    EXPECT_EQ(result, true);

    script.fini();
} 

    