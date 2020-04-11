#include "environment.h"

#include "core/assert.h"

namespace script_system {

	void Environment::define(const string& name, core::Value value)
	{
		auto res = values_.emplace(name, value);
		ASSERT(res.second);
	}

	core::Value Environment::get(parser::Token token)
	{
		auto it = values_.find(token.lexeme);
		if (it != values_.end())
			return it->second;

		ASSERT(false);
	}

}
