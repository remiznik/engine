#pragma once
#include "types.h"

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
		explicit AnyValue(const string& s) : s(s) {};
        ~AnyValue()
        {}

        void reset()
        {
			memset(this, 0, sizeof(AnyValue)); 
        }

        int i;
        double d;
        string s;
    };

    enum class ValueType
    {
        None,
        Integer,
        Double,
        String
    };
}