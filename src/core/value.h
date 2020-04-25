#pragma once

#include "core/types.h"
#include "core/any_value.h"


namespace core
{
    class Value
    {
    public:
        Value();
		Value(const Value& other);
		Value& operator=(const Value& other);

        Value(const string& value);        
        Value(double value);
		Value(int value);
		Value(bool value);
		Value(const shared_ptr<Object>& obj);
		

		~Value()
		{
			freeData();
		}

		ValueType type() const { return type_; }

		bool operator==(const Value& other) const;
		Value operator+(const Value& other) const;

		template<typename T> T get() const;		

		template<typename T> T to() const;

	private:
		string toChars() const;
		double toDouble() const;
		int toInt() const;
		bool toBool() const;
		shared_ptr<Object> toObj() const;
		void freeData();

    private:
      ValueType type_ { ValueType::None };
      AnyValue value_; 
    };

        
	template<> inline string				Value::to<string>() const { return toChars(); }

	template<> inline double 				Value::get<double>() const { return toDouble(); }
	template<> inline bool 					Value::get<bool>() const { return toBool(); }
	template<> inline int					Value::get<int>() const { return toInt(); }
	template<> inline string				Value::get<string>() const { return toChars(); }
	template<> inline shared_ptr<Object>	Value::get<shared_ptr<Object>>() const { return toObj(); }
}

