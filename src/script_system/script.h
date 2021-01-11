#pragma once

#include "core/types.h"

namespace script_system {
	class NativeFunction;
	
	class Script
	{
	public:
		bool init();
		void fini();

		bool run(const char* path);

		bool registreFunction(const char* name, const shared_ptr<NativeFunction>& function);

	private:
		map<string, shared_ptr<NativeFunction>> functions_;

	};
}