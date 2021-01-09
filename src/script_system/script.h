#pragma once

namespace script_system {
	class Script
	{
	public:
		void init();
		void fini();
		bool run(const char* path);
	};
}