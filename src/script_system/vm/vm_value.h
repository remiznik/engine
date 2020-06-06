#pragma once

#include "vm/common.h"

namespace script_system {
namespace vm {
//    typedef double Value;

    typedef struct vm_value
    {
        int count;
        int capacity;
        script_system::vm::Value* values;
    } ValueArray;

    void initValueArray(ValueArray* array);              
    void writeValueArray(ValueArray* array, Value value);
    void freeValueArray(ValueArray* array);      

    void printValue(Value value);
}
}