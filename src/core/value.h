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
		Value(bool value);
		

		~Value()
		{
			freeData();
		}

		template<typename T> T get() const;		

		template<typename T> T to() const;

	private:
		string toChars() const;
		double toDouble() const;
		bool toBool() const;
		void freeData();

    private:
      ValueType type_ { ValueType::None };
      AnyValue value_; 
    };

        
	template<> inline string		Value::to<string>() const { return toChars(); }

	template<> inline double 		Value::get<double>() const { return toDouble(); }
	template<> inline bool 			Value::get<bool>() const { return toBool(); }
}

