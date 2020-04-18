#include <fstream>
#include <streambuf>

#include "file_reader.h"

namespace core {

    string FileReader::read(const string& path)
    {
        std::ifstream t(path);
        std::string str((std::istreambuf_iterator<char>(t)),
                            std::istreambuf_iterator<char>());
        return str;
    }

}