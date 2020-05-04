#pragma once

#include "types.h"

namespace core
{

    class LogMessage
    {
    public:
        LogMessage()
            : text_(" text not set ")
        {}

        LogMessage(const string& text)
            : text_(text)
        {}
        
        virtual string toString() { return text_; }
    private:
        string text_;
    };

    class Logger
    {
    public:
        Logger();       
        
        void write(LogMessage&& messgae);
    };
}