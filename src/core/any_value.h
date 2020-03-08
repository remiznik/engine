#pragma once
#include "types.h"

namespace core 
{
   union AnyValue
    {
        AnyValue()
        {            
        }     
        ~AnyValue()
        {}

        void reset()
        {
            
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