#pragma once

#include "vm/chunk.h"
#include "vm/object.h"

namespace script_system {
namespace vm {	

	void markCompilerRoots();
	ObjFunction* compile(const char* source);
	
}
}