#include "logger.h"

#include <iostream>

namespace core
{
    Logger::Logger()
    {
        
    }

    void Logger::write(LogMessage& messgae)
    {
        std::cout << messgae.toString() << std::endl;
    }

    void Logger::write(const string& message)
    {
        std::cout << message << std::endl;
    }
}