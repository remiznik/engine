#pragma once

#include "vm/chunk.h"
#include "vm/table.h"
#include "vm/object.h"

#define FRAMES_MAX 64
#define STACK_MAX (FRAMES_MAX * UINT8_COUNT)

namespace script_system {
namespace vm {

typedef struct {
    ObjClosure* closure;
    uint8_t* ip;
    Value* slots;
} CallFrame;

typedef struct vm
{
    CallFrame frames[FRAMES_MAX];
    int frameCount;
    Value stack[STACK_MAX];
    Value* stackTop;
    Table globals;
    Table strings;
    ObjString* initString;
    ObjUpvalue* openUpvalues;

    size_t bytesAllocated;
    size_t nextGC;

    Obj* objects;

    int grayCount;
    int grayCapacity;
    Obj** grayStack;
} VM;

typedef enum {            
  INTERPRET_OK,           
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR 
} InterpretResult;   


extern VM vm;

void initVM();
void freeVM();



void push(Value value);
Value pop();

InterpretResult interpret(const char* chunk);
void defineNative(const char* name, NativeFn function);
bool call(const char* name);
}
}