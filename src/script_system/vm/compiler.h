#pragma once

#include "vm/chunk.h"

namespace script_system {
namespace vm {

ObjFunction* compile(const char* source, Chunk* chunk);
	
}
}