#pragma once

#include "types.h"

namespace core {
    class FileSystem
    {
    public: 
        string readFile(const string& patr) const;
    };
}