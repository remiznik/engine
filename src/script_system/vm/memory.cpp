#include "vm/memory.h"

#include "vm/object.h"
#include "vm/vm.h"

#include <cstdlib>

namespace script_system {
namespace vm {
    namespace memory {


        void freeObject(Obj* object)
        {
            switch (object->type)
            {
            case OBJ_FUNCTION:
            {
                ObjFunction* function = (ObjFunction*)object;
                freeChunk(&function->chunk);
                FREE(ObjFunction, object);
                break;
            }
            case OBJ_NATIVE:
            {
                FREE(ObjNative, object);
                break;
            }
            case OBJ_STRING:
                ObjString* string = (ObjString*)object;
                FREE_ARRAY(char, string->chars, string->length + 1);
                FREE(ObjString, object);
                break;
            }
        }

        void* reallocate(void* previous, size_t oldSize, size_t newSize)
        {
            if (newSize == 0)
            {
                free(previous);
                return nullptr;
            }
            return realloc(previous, newSize);
        }

        void freeObjects()
        {
            Obj* object = vm.objects;
            while (object != nullptr)
            {
                Obj* next = object->next;
                freeObject(object);
                object = next;
            }
        }
}
}
}