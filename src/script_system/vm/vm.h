#pragma once

#include "chunk.h"

#define STACK_MAX 256

namespace script_system {
namespace vm {
typedef struct vm
{
    Chunk* chunk;
    uint8_t* ip;
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum {            
  INTERPRET_OK,           
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR 
} InterpretResult;   


void initVM();
void freeVM();

void push(Value value);
Value pop();

InterpretResult interpret(const char* chunk);
	
}
}