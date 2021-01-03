#pragma once
#include <stddef.h>

#include "vm/value.h"

#define ALLOCATE(type, count) \
    (type*)script_system::vm::memory::reallocate(nullptr, 0, sizeof(type) * (count))

#define FREE(type, pointer) script_system::vm::memory::reallocate(pointer, sizeof(type), 0)

#define GROW_CAPACITY(capacity)\
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(previous, type, oldCount, count)\
    (type*)script_system::vm::memory::reallocate(previous, sizeof(type) * (oldCount),\
        sizeof(type) * (count)) 

#define FREE_ARRAY(type, pointer, oldCount) \
    script_system::vm::memory::reallocate(pointer, sizeof(type) * (oldCount), 0)



namespace script_system {
namespace vm {
namespace memory {

    void* reallocate(void* previous, size_t oldSize, size_t newSize);    
    void markObject(Obj* object);
    void markValue(Value value);
    void collectGarbage();
    void freeObjects();

 

}
}
}