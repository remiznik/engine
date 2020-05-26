#include "utils.h"


namespace script_system {
namespace vm {
namespace utils {
    void convertIntTwoUint(const int val, uint8_t& a, uint8_t& b)
    {
        a = (val >> 8) & 0x00ff;
        b = val & 0x00ff;
    }

    void convertTwoUintInt(const uint8_t a, const  uint8_t b, int& result)
    {
        result = (a << 8) + b;
    }

}
}
}