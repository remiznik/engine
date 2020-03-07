#pragma once

#include "types.h"

namespace core
{

    class LogMessage
    {
    public:
        virtual string toString() = 0;
    };

    class Logger
    {
    public:
        Logger();
        
        void write(LogMessage& messgae);
    };
}