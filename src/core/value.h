#pragma once

#include "core/types.h"
#include "core/any_value.h"


namespace core
{
    class Value
    {
    public:
        Value();
		Value(const Value& othe);

        Value(const string& value);        
        Value(double value);
		Value(int value);
		


		~Value()
		{
			freeData();
		}

		template<typename T> T to() const;

	private:
		string toChars() const;
		void freeData();

    private:
      ValueType type_ { ValueType::None };
      AnyValue value_; 
    };

        
	template<> inline string		Value::to<string>() const { return toChars(); }
}

