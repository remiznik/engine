#include "class_def.h"

#include "core/m_assert.h"
#include "interpreter.h"

namespace script_system {


InFunction::InFunction(parser::Interpreter* inter, parser::Function* expr, const shared_ptr<Environment>& closure, bool initializer)
	: inter_(inter), expr_(expr), closure_(closure), isInitializer_(initializer)
{
}

core::Value InFunction::call(const vector<core::Value>& args)
{
	auto env = makeShared<Environment>(closure_);
	ASSERT(args.size() == expr_->params.size(), "Not equal arguments .")
	
	for (size_t i = 0; i < expr_->params.size(); ++i)
	{
		env->define(expr_->params[i].lexeme, args[i]);
	}
	try
	{
		inter_->execute(expr_->body, env);
	}
	catch(const RetrunException& e)
	{
		if (isInitializer_) return closure_->getAt(0, "this");
		return e.value;
	}
	if (isInitializer_) return closure_->getAt(0, "this");
	return core::Value();
}

shared_ptr<InFunction> InFunction::bind(const shared_ptr<InClassInstance>& instance)
{	
	auto enviroment = makeShared<Environment>(closure_);
	// dengorus 
	enviroment->define("this", core::Value(instance));
	return makeShared<InFunction>(inter_, expr_, enviroment, isInitializer_);
}


string InFunction::toString() const 
{
	string result;
	result.append(" Function ");
	result.append(expr_->name.lexeme);
	result.append("\n");
	return result;
}

core::Value InClass::call(const vector<core::Value>& args)
{
	auto instance = makeShared<InClassInstance>(this);
	auto it = methods_.find("init");
	if (it != methods_.end())
	{
		it->second->bind(instance)->call(args);
	}

	return core::Value(instance);
}

shared_ptr<InFunction> InClass::method(const string& name)
{
	auto it = methods_.find(name);
	if (it != methods_.end())
	{
		return it->second;
	}

	if (supperClass_ != nullptr)
	{
		return supperClass_->method(name);
	}
	return nullptr;
}

string InClass::toString() const
{
	string result;
	result.append(" Class ");
	result.append(name_);
	result.append("\n");
	return result;
}

string InClassInstance::toString() const 
{
	string result;
	result.append(" Instence of ");
	result.append(class_->toString());
	for (auto field : fields_)
	{
		result.append(field.first);
		result.append("   ");
		result.append(field.second.to<string>());
		result.append("\n");
	}
	return result;
}

core::Value InClassInstance::get(const string& name)
{
	auto it = fields_.find(name);
	if (it != fields_.end())
	{
		return it->second;
	}
	auto method = class_->method(name);
	if (method != nullptr)
	{
		auto n = method->bind(shared_from_this());
		return core::Value(n);
	}

	ASSERT(false, "Undefined property .");
}

void InClassInstance::set(const string& name, core::Value value)
{
	fields_.emplace(name, value);
}

	
}