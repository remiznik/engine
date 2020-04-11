#pragma once

#include "core/value.h"

#include "token.h"

namespace script_system {
	class Environment
	{
	public:		
		void define(const string& name, core::Value value);
		core::Value get(parser::Token token);
		void assign(parser::Token name, core::Value value);

	private:
		map<string, core::Value> values_;
	};
}