#include "class_def.h"

#include "core/assert.h"

namespace script_system {

core::Value InClass::call(const vector<core::Value>& args)
{
	auto instance = makeShared<InClassInstance>(this);
	return core::Value(instance);
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

core::Value InClassInstance::get(const string& name) const
{
	auto it = fields_.find(name);
	if (it != fields_.end())
	{
		return it->second;
	}

	ASSERT(false, "Undefined property .");
}

void InClassInstance::set(const string& name, core::Value value)
{
	fields_.emplace(name, value);
}
	
}