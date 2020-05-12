#include "value.h"
#include "value_impl.h"

namespace core
{
    Value::Value()
        : type_(ValueType::None)
    {}
		
    Value::Value(double value)
        : type_(ValueType::Double), value_(value)
    {		
	}

	Value::Value(int value)
		: type_(ValueType::Integer), value_(value)
	{		
	}

	Value::Value(const char* value)
		: type_(ValueType::String), value_(copy(value))
    {
    }

    Value::Value(bool value)
        : type_(ValueType::Bool), value_(value)
    {
    }

	Value::Value(const shared_ptr<Object>& obj)
		: type_(ValueType::Object), value_(obj)
	{		
	}

    Value::Value(const Value& other)
    {
        getValueImpl(type_)->freeValue(value_);

        getValueImpl(other.type_)->copyValue(other.value_, value_);

        type_ = other.type_;
    }

	Value& Value::operator=(const Value& other)
	{
		getValueImpl(type_)->freeValue(value_);

        getValueImpl(other.type_)->copyValue(other.value_, value_);

        type_ = other.type_;        

		return *this;
	}

	bool Value::operator==(const Value& other) const 
	{
		return getValueImpl(type_)->equal(*this, other);
	}

	Value Value::operator+(const Value& other) const
	{
		return getValueImpl(type_)->plus(*this, other);
	}

	void Value::freeData()
	{
		getValueImpl(type_)->freeValue(value_);
	}

	string Value::toChars() const
	{
		return getValueImpl(type_)->toChars(value_);
	}

    double Value::toDouble() const
	{
		return getValueImpl(type_)->toDouble(value_);
	}

    bool Value::toBool() const 
    {
        return getValueImpl(type_)->toBool(value_);
    }

	int Value::toInt() const
	{
		return getValueImpl(type_)->toInt(value_);
	}

	shared_ptr<Object> Value::toObj() const 
	{
		return getValueImpl(type_)->toObj(value_);
	}

	const char* Value::copy(const char* value)
	{
		static set<string> strings;
		auto it = strings.find(value);
		if (it != strings.end())
		{
			return it->c_str();
		}
		else
		{
			auto res = strings.emplace(value);
			return res.first->c_str();
		}

	}
}