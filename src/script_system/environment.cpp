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

	core::Value Environment::get(const string& name)
	{
		auto it = values_.find(name);
		if (it != values_.end())
			return it->second;
		
		if (enclosing_) return enclosing_->get(name);

		ASSERT(false, "Enviroment::get");
	}

	void Environment::assign(const string& name, core::Value value)
	{
		auto it = values_.find(name);
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

	void Environment::assignAt(int distance, const string& name, core::Value value)
	{
		ancestor(distance)->values_[name] = value;
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
		return env;
	}

	shared_ptr<Environment> Environment::enclosing() const 
	{
		return enclosing_;
	}

	string Environment::toString(const string& shift)
	{
		string result;
		if (enclosing_)
		{
			result.append(enclosing_->toString(shift + "---"));
		}

		for( auto v : values_)
		{
			result.append(shift);
			result.append(v.first);
			result.append("  ");
			result.append(v.second.to<string>());
			result.append("\n");
		}		
		
		return result;

	}

}
