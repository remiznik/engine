#include <fstream>
#include <streambuf>

#include "file_system.h"

namespace core {

    string FileSystem::readFile(const string& path) const 
    {
        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
        return str;
    }

}