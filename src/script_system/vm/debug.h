#pragma once

#include "chunk.h"

namespace script_system {
namespace vm {
	void disassembleChunk(Chunk* chunk, const char* name);
	int disassebleInstruction(Chunk* chunk, int offset);
}
}