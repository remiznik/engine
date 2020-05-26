#pragma once

#include "chunk.h"

namespace script_system {
namespace vm {
typedef struct vm
{
    Chunk* chunk;
    uint8_t* ip;
} VM;

typedef enum {            
  INTERPRET_OK,           
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR 
} InterpretResult;   


void initVM();
void freeVM();

InterpretResult interpret(Chunk* chunk);
	
}
}