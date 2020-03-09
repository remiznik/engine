#pragma once

#include "core/types.h"
#include "core/any_value.h"


namespace core
{
    class Value
    {
    public:
        Value();
        Value(const string& value);
        Value(const Value& othe);
        Value(double value);


    private:
      ValueType type_ { ValueType::None };
      AnyValue value_; 
    };
}