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

		~Value()
		{
			freeData();
		}

	private:
		void freeData();

    private:
      ValueType type_ { ValueType::None };
      AnyValue value_; 
    };
}