#include "script_system/script.h"

#include "vm/vm.h"
#include "script_system/native_function.h"

#include <functional>

namespace script_system {

	bool Script::init()
	{
		vm::initVM();
		return true;
	}
	
	void Script::fini()
	{
		vm::freeVM();
	}

	bool Script::run(const char* source)
	{	
		auto result = vm::interpret(source);

		return result == vm::INTERPRET_OK;
	}

	bool Script::registreFunction(const char* name, const shared_ptr<NativeFunction>& function)
	{
		auto result = functions_.emplace(name, function);

		vm::defineNative(name, std::bind(&NativeFunction::call, function.get(), std::placeholders::_1, std::placeholders::_2));

		return result.second;
	}
}