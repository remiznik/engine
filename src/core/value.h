#pragma once
#include "core/types.h"

namespace core
{
    class Value
    {
    public:
        Value();
        Value(const string& value);

    private:
        string value_;
    };
}