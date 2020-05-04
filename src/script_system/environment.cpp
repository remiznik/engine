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
			return;
		}
		
		ASSERT(false, "Enviroment::assign");
	}

	core::Value Environment::getAt(int distance, const string& name)
	{
		return ancestor(distance)->values_[name];
	}

	Environment* Environment::ancestor(int distance)
	{
		auto env = this;
		for (int i = 0; i < distance; ++i)
		{
			env = env->enclosing_.get();
		}

		return this;
	}

}
