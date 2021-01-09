#pragma once

#include <cstdint>

namespace script_system {
namespace vm {
namespace utils {

    void convertIntTwoUint(const int val, uint8_t& a, uint8_t& b);

    void convertTwoUintInt(const uint8_t a, const  uint8_t b, int& result);
}    
}
}