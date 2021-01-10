#include "script_system/native_function.h"

#include <time.h>

namespace script_system 
{
    vm::Value ClockFunction::call(int argCount, vm::Value* args)
    {
        using namespace vm;
        return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
    }
}