#pragma once

#include "core/value.h"

#include "token.h"

namespace script_system {
	class Environment
	{
	public:		
		Environment()
		{}
		Environment(const shared_ptr<Environment>& enclosing);

		void define(const string& name, core::Value value);

		core::Value get(parser::Token token);
		core::Value getAt(int distance, const string& name);

		void assign(parser::Token name, core::Value value);
		void assignAt(int distance, parser::Token name, core::Value value);

		string toString(const string& shift);

	private:
		Environment* ancestor(int distance);

	private:
		shared_ptr<Environment> enclosing_{nullptr};
		map<string, core::Value> values_;
	};
}