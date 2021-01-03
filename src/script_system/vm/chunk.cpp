
#include "vm/chunk.h"
#include "vm/vm.h"

#include "memory.h"
#include "utils.h"

namespace script_system {
namespace vm {

	void initChunk(Chunk* chunk)
	{
		chunk->count = 0;
		chunk->capacity = 0;
		chunk->code = nullptr;
		chunk->lines = nullptr;
		initValueArray(&chunk->constants);
	}

	void writeChunk(Chunk* chunk, uint8_t byte, int line)
	{
		if (chunk->capacity  < chunk->count + 1)
		{
			int oldCapacity = chunk->capacity;
			chunk->capacity = GROW_CAPACITY(oldCapacity);
			chunk->code = GROW_ARRAY(chunk->code, uint8_t, oldCapacity, chunk->capacity);
			chunk->lines = GROW_ARRAY(chunk->lines, int,  oldCapacity, chunk->capacity);  
		}
		chunk->code[chunk->count] = byte;
		chunk->lines[chunk->count] = line;
		chunk->count++;
	}

	int addConstant(Chunk* chunk, Value value)
	{
		push(value);
		writeValueArray(&chunk->constants, value);
		pop();
		return chunk->constants.count - 1;
	}

	void writeConstant(Chunk* chunk, Value value, int line)
	{
		writeChunk(chunk, OP_CONSTANT_LONG, line);
		int constant = addConstant(chunk, value);
		uint8_t a, b;		
		utils::convertIntTwoUint(constant, a, b);
		writeChunk(chunk, a, line);
		writeChunk(chunk, b, line);
	}

	void freeChunk(Chunk* chunk)
	{
		FREE_ARRAY(int, chunk->lines, chunk->capacity);
		FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
		freeValueArray(&chunk->constants);
		initChunk(chunk);
	}
}
}