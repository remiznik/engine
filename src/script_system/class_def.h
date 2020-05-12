#pragma once

// using this foc InClassInstance, make shared form this.
#include <memory>

#include "core/object.h"
#include "core/value.h"

#include "environment.h"

namespace script_system {
namespace parser {
	class Interpreter;
	class Function;
}

class InClassInstance;

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
	InFunction(parser::Interpreter* inter, parser::Function* expr, const shared_ptr<Environment>& closure, bool initialize );

	virtual string toString() const override;

	core::Value call(const vector<core::Value>& args) override;

	// very cerful maby cicle pointer. method depent from instance
	shared_ptr<InFunction> bind(const shared_ptr<InClassInstance>& instance);

private:
	bool isInitializer_{ false };
	parser::Interpreter* inter_ {nullptr};
	parser::Function* expr_{nullptr};
	shared_ptr<Environment> closure_{nullptr};
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

class InClassInstance : public core::Object, public std::enable_shared_from_this<InClassInstance>
{
public:
	InClassInstance(InClass* c)
		: class_(c)
	{}

	InClassInstance(InClass* c, const map<string, core::Value>& f)
		: class_(c), fields_(f)
	{}

	virtual string toString() const override;

	core::Value get(const string& name);
	void set(const string& name, core::Value val);
	
private:
	InClass* class_;
	map<string, core::Value> fields_;
};

}