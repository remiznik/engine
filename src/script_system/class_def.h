#pragma once

#include "core/object.h"
#include "core/value.h"

namespace script_system {

class Callable : public core::Object
{
public:
	virtual core::Value call(const vector<core::Value>& args) = 0;
};

class InClass : public Callable
{
public:
	InClass(const string& n)
		: name_(n)
	{}

	virtual core::Value call(const vector<core::Value>& args) override;	
	virtual string toString() const override;
private:
	string name_;
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