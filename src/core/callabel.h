#pragma once

#include "object.h"
#include "value.h"

namespace core {
    class Callable : public Object
    {
    public:
        virtual core::Value call(const vector<Value>& args) = 0;
    };
}