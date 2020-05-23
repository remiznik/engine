#include "vm_memory.h"
#include <cstdlib>

namespace script_system {
namespace vm {
namespace memory {
    void* reallocate(void* previous, size_t oldSize, size_t newSize)
    {
        if (newSize == 0)
        {
            free(previous);
            return nullptr;
        }
        return realloc(previous, newSize);;
    }
}
}
}