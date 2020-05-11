#pragma once

#include "core/file_system.h"
#include "core/logger.h"

#include "scanner.h"
#include "interpreter.h"

namespace script_system
{
	class Script
	{
	public:
		Script(const core::FileSystem& fileSystem);

		bool initialize();
		void update(int st, int x, int y);
		void registreFunction(const string& name, const shared_ptr<class core::Callable>& fnc);

	private:
		const core::FileSystem& fileSystem_;
		Scanner scanner_;
		parser::Interpreter interpreter_;
		core::Logger logger_;
	};
}
