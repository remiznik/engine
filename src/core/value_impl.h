#pragma once
#include "core/any_value.h"

namespace core
{
    class ValueImpl
    {
    public:
        virtual void freeValue(AnyValue& value) = 0;
        virtual void copyValue(const AnyValue& from, AnyValue& value) = 0;
		virtual string toChars(const AnyValue& value) = 0;
    };

    ValueImpl* getValueImpl(ValueType type);
}