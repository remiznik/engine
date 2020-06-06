#pragma once

#include "vm/chunk.h"

namespace script_system {
namespace vm {

bool compile(const char* source, Chunk* chunk);
	
}
}