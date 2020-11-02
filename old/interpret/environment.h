#pragma once

#include "core/value.h"


namespace script_system {
	class Environment
	{
	public:		
		Environment()
		{}
		Environment(const shared_ptr<Environment>& enclosing);

		void define(const string& name, core::Value value);

		core::Value get(const string& name);
		core::Value getAt(int distance, const string& name);

		void assign(const string& name, core::Value value);
		void assignAt(int distance, const string& name, core::Value value);

		string toString(const string& shift);

		shared_ptr<Environment> enclosing() const;

	private:
		Environment* ancestor(int distance);

	private:
		shared_ptr<Environment> enclosing_{nullptr};
		map<string, core::Value> values_;
	};
}