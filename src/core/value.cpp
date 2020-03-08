#include "value.h"

namespace core
{
    Value::Value()
    {}

    Value::Value(const string& value)
        : value_(value)
    {}
}