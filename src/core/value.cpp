#include "value.h"

namespace core
{
    Value::Value()
        : type_(ValueType::None)
    {}

    Value::Value(const string& value)
        : type_(ValueType::String)
    {}

    Value::Value(const Value& other)
    {
        type_ = other.type_;
        //value_ = other.value_;
    }
}