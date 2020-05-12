#pragma once
#include "types.h"
#include "object.h"

#include <cstring>

namespace core 
{
   union AnyValue
    {
        AnyValue()
        {            
			reset();
        }     

		explicit AnyValue(int i) : i(i) {};
		explicit AnyValue(double d) : d(d) {};
		explicit AnyValue(const char* s) : s(s) {};
        explicit AnyValue(bool b) : b(b) {}
        explicit AnyValue(const shared_ptr<Object>& o) : obj(o) {}
        ~AnyValue()
        {}

        void reset()
        {
			memset(this, 0, sizeof(AnyValue)); 
        }

        bool b;
        int i;
        double d;
        const char* s;
        shared_ptr<Object> obj;
    };

    enum class ValueType
    {
        None,
        Bool,
        Integer,
        Double,
        String,
        Object
    };
}