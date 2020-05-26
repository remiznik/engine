#include "vm.h"
#include "vm_value.h"

#include "common.h"

#include <stdio.h>

namespace script_system {
namespace vm {
VM vm;
void initVM() 
{

}

void finiVM()
{

}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE() ]);
    
    for(;;)
    {
        uint8_t inistruction;
        switch (inistruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            printValue(constant);
            printf("\n");
        }
        case OP_RETURN:
            return INTERPRET_OK;
            break;
        
        default:
            break;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
}

InterpretResult interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}   

}
}
