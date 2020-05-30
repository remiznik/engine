#pragma once
#include "common.h"
#include "vm_value.h"

namespace script_system {
namespace vm {
	typedef enum {
		OP_CONSTANT_LONG,
		OP_CONSTANT,
		OP_NEGATE,
		OP_ADD,     
  		OP_SUBTRACT,
  		OP_MULTIPLY,
  		OP_DIVIDE,
  		OP_RETURN,  		       
	} OpCode;   

	typedef struct  {
		int count;
		int capacity;
		int* lines;
		uint8_t* code;		
		ValueArray constants;
	} Chunk;

	void initChunk(Chunk* chunk);	
	void writeChunk(Chunk* chunk, uint8_t byte, int line);
	void writeConstant(Chunk* chunk, Value value, int line);
	int addConstant(Chunk* chunk, Value value);
	void freeChunk(Chunk* chunk);
}
}