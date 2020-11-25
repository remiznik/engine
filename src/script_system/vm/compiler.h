#pragma once

#include "vm/chunk.h"
#include "vm/object.h"

namespace script_system {
namespace vm {

ObjFunction* compile(const char* source);
	
}
}