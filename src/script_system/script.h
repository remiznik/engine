#pragma once

#include "core/file_reader.h"
#include "core/logger.h"

#include "scanner.h"
#include "interpreter.h"

namespace script_system
{
	class Script
	{
	public:
		Script();

		void run();

	private:
		core::Logger logger_;
		core::FileReader reader_;
		Scanner scanner_;
		parser::Interpreter interpreter_;		
	};
}
