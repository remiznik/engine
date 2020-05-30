#include "vm.h"
#include "vm_value.h"
#include "vm_debug.h"

#include "common.h"

#include <stdio.h>

namespace script_system {
namespace vm {
VM vm;

static void resetStack()
{
    vm.stackTop = vm.stack;
}

void push(Value value)
{
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop()
{
    vm.stackTop--;
    return *vm.stackTop;
}

void initVM() 
{
    resetStack();
}

void finiVM()
{

}

static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE() ]);
#define BINARY_OP(op) \
    do { \
      double b = pop(); \
      double a = pop(); \
      push(a op b); \
    } while (false) 
    
    for(;;)
    {
#ifdef DEBUG_TRACE_EXECUTION
        printf("         ");
        for (Value* slot  = vm.stack; slot  < vm.stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        disassebleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif
        uint8_t inistruction;
        switch (inistruction = READ_BYTE())
        {
        case OP_CONSTANT:
        {
            Value constant = READ_CONSTANT();
            push(constant);
            break;
        }
        case OP_ADD:      BINARY_OP(+); break;
        case OP_SUBTRACT: BINARY_OP(-); break;
        case OP_MULTIPLY: BINARY_OP(*); break;
        case OP_DIVIDE:   BINARY_OP(/); break;
        case OP_NEGATE:   push(-pop()); break;
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
            break;
        }
        
        default:
            break;
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk)
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}   

}
}
