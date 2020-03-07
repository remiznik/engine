#pragma once

#include "core/logger.h"

#include "scanner.h"

namespace script_system
{
	class Script
	{
	public:
		Script();

	private:
		Scanner scanner_;
		core::Logger logger_;
	};
}
