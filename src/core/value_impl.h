#pragma once
#include "core/any_value.h"

namespace core
{
	class Value;
    class ValueImpl
    {
    public:
        virtual void freeValue(AnyValue& value) const  = 0;
        virtual void copyValue(const AnyValue& from, AnyValue& value) const = 0;
		virtual string toChars(const AnyValue& value) const = 0;
        virtual double toDouble(const AnyValue& value) const = 0;
		virtual int toInt(const AnyValue& value) const = 0;
        virtual bool toBool(const AnyValue& value) const = 0;
		virtual bool equal(const Value& self, const Value& other) const = 0;
		virtual Value plus(const Value& self, const Value& other) const = 0;
    };

    ValueImpl* getValueImpl(ValueType type);
}