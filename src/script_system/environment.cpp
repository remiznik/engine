#include "environment.h"

#include "core/assert.h"

#include <iostream>

namespace script_system {

	Environment::Environment(const shared_ptr<Environment>& enclosing)
		: enclosing_(enclosing)
	{

	}

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
		
		if (enclosing_) return enclosing_->get(token);

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
		if (enclosing_)
		{
			enclosing_->assign(name, value);
			std::cout << " assing " << name.lexeme << std::endl;
			return;
		}
		
		ASSERT(false, "Enviroment::assign");
	}

}
