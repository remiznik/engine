#include "class_def.h"

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
	return result;
}
	
}