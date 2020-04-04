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
        explicit AnyValue(bool b) : b(b) {}
        ~AnyValue()
        {}

        void reset()
        {
			memset(this, 0, sizeof(AnyValue)); 
        }

        bool b;
        int i;
        double d;
        string s;
    };

    enum class ValueType
    {
        None,
        Bool,
        Integer,
        Double,
        String
    };
}