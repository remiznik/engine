#include "script_system/script.h"

#include "vm/vm.h"

#include "core/file_reader.h"

namespace script_system {

	void Script::init()
	{
		vm::initVM();
	}
	
	void Script::fini()
	{
		vm::freeVM();
	}

	bool Script::run(const char* path)
	{
		core::FileReader reader;
		auto text = reader.read(path);

		auto result = vm::interpret(text.c_str());

		return result == vm::INTERPRET_OK;
	}
}