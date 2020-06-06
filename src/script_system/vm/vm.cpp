#include "vm.h"
#include "vm_value.h"
#include "vm_debug.h"
#include "compiler.h"

#include "vm/common.h"

#include <stdio.h>
#include <stdarg.h>

namespace script_system {
namespace vm {
VM vm;

namespace {

    void resetStack()
    {
        vm.stackTop = vm.stack;
    }

    void runtimeError(const char* format, ...) 
    {
        va_list args;                                    
        va_start(args, format);                          
        vfprintf(stderr, format, args);                  
        va_end(args);                                    
        fputs("\n", stderr);                             

        size_t instruction = vm.ip - vm.chunk->code - 1; 
        int line = vm.chunk->lines[instruction];         
        fprintf(stderr, "[line %d] in script\n", line);  

        resetStack();                                    
        }          
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

Value peek(int distance)
{
    return vm.stackTop[-1-distance];
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
        case OP_NEGATE:   
            if (!IS_NUMBER(peek(0))) 
            {                  
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;           
            }                                           

            push(NUMBER_VAL(-AS_NUMBER(pop())));        
            break;
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

InterpretResult interpret(const char* source)
{
    Chunk chunk;
    initChunk(&chunk);
    if (!compile(source, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);    
    return result;
}   

}
}
