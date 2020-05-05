#pragma once

#include "types.h"

namespace core {
    class Object
    {
    public:
        virtual ~Object() = default;
        virtual string toString() const = 0;
    };
}