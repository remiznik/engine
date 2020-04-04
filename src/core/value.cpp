#include "value.h"
#include "value_impl.h"

namespace core
{
    Value::Value()
        : type_(ValueType::None)
    {}
		
    Value::Value(double value)
        : type_(ValueType::Double)
    {
		value_.d = value;
	}

	Value::Value(int value)
		: type_(ValueType::Integer)
	{
		value_.i = value;
	}

    Value::Value(const string& value)
        : type_(ValueType::String)
    {}

    Value::Value(const Value& other)
    {
        getValueImpl(type_)->freeValue(value_);

        getValueImpl(other.type_)->copyValue(other.value_, value_);

        type_ = other.type_;        
    }

	void Value::freeData()
	{
		getValueImpl(type_)->freeValue(value_);
	}

	string Value::toChars() const
	{
		return getValueImpl(type_)->toChars(value_);
	}
}