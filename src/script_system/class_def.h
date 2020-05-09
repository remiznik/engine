#pragma once

#include "core/object.h"
#include "core/value.h"

#include "environment.h"

namespace script_system {
namespace parser {
	class Interpreter;
	class Function;
}

class RetrunException : public std::exception
{
public:
	RetrunException(core::Value v)
		: value(v)
	{}	
	core::Value value;
};


class Callable : public core::Object
{
public:
	virtual core::Value call(const vector<core::Value>& args) = 0;
};

class InFunction : public Callable
{
public:
	InFunction(parser::Interpreter* inter, parser::Function* expr, const shared_ptr<Environment>& closure);

	core::Value call(const vector<core::Value>& args) override;
	virtual string toString() const override;

private:
	parser::Interpreter* inter_;
	parser::Function* expr_;
	shared_ptr<Environment> closure_;
};


class InClass : public Callable
{
public:
	InClass(const string& n, const map<string, shared_ptr<InFunction>>& m)
		: name_(n), methods_(m)
	{}

	virtual core::Value call(const vector<core::Value>& args) override;	
	virtual string toString() const override;

	shared_ptr<InFunction> method(const string& name);

private:
	string name_;
	map<string, shared_ptr<InFunction>> methods_;
};

class InClassInstance : public core::Object
{
public:
	InClassInstance(InClass* c)
		: class_(c)
	{}
	virtual string toString() const override;

	core::Value get(const string& name) const;
	void set(const string& name, core::Value val);

private:
	InClass* class_;
	map<string, core::Value> fields_;
};

}