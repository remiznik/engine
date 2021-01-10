#pragma once

#include "script_system/vm/value.h"

namespace script_system 
{

    class NativeFunction
    {
    public: 
        virtual vm::Value call(int, vm::Value*) = 0;
    };

    class ClockFunction : public NativeFunction
    {
    public:
        vm::Value call(int argCount, vm::Value* args) override;
    };

}