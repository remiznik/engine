#include "environment.h"

#include "core/assert.h"

#include <iostream>

namespace script_system {

	void Environment::define(const string& name, core::Value value)
	{
		auto res = values_.emplace(name, value);
		ASSERT(res.second, "Enviroment::define");
	}

	core::Value Environment::get(parser::Token token)
	{
		auto it = values_.find(token.lexeme);
		if (it != values_.end())
			return it->second;

		std::cout << " get  " << token.toString() << std::endl;
		ASSERT(false, "Enviroment::get");
	}

	void Environment::assign(parser::Token name, core::Value value)
	{
		auto it = values_.find(name.lexeme);
		if (it != values_.end())
		{
			it->second = value;
			return;
		}
		std::cout << "assign " << name.lexeme << std::endl;
		ASSERT(false, "Enviroment::assign");
	}

}
