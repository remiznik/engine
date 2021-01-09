#pragma once

#include "core/file_reader.h"
#include "core/logger.h"

#include "scanner.h"
#include "interpreter.h"

#include "vm/vm.h"

namespace script_system
{
	class Script
	{
	public:
		Script();

		void run(const string& fileName);		
		void execute(const string& scrip);

		void registreFunction(const string& name, const shared_ptr<class core::Callable>& fnc);

	private:

	private:
		core::Logger logger_;
		core::FileReader reader_;
		Scanner scanner_;
		parser::Interpreter interpreter_;		
	};
}
