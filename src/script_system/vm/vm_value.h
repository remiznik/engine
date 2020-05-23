#pragma once

namespace script_system {
namespace vm {
    typedef double Value;

    typedef struct vm_value
    {
        int count;
        int capacity;
        Value* values;
    } ValueArray;

    void initValueArray(ValueArray* array);              
    void writeValueArray(ValueArray* array, Value value);
    void freeValueArray(ValueArray* array);      

    void printValue(Value value);
}
}