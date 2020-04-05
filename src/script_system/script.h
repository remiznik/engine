#pragma once

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
		Scanner scanner_;
		parser::Interpreter interpreter_;
		core::Logger logger_;
	};
}
