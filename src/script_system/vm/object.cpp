#include "vm/object.h"

#include "core/types.h"
#include "vm/memory.h"
#include "vm/vm.h"

#include <cstring>

namespace script_system {
	namespace vm {

#define ALLOCATE_OBJ(type, objectType) \
		(type*)allocateObject(sizeof(type), objectType)

		Obj* allocateObject(size_t size, ObjType type)
		{
			Obj* object = (Obj*)script_system::vm::memory::reallocate(nullptr, 0, size);
			object->type = type;
			object->isMarked = false;

			object->next = vm.objects;
			vm.objects = object;
#ifdef DEBUG_LOG_GC
			printf("%p allocate %zd for %d\n", (void*)object, size, type);
#endif

			return object;
		}
		
		ObjFunction* newFunction()
		{
			ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);

			function->arity = 0;
			function->upvalueCount = 0;
			function->name = nullptr;
			initChunk (&function->chunk);
			return function;
		}

		ObjClosure* newClosure(ObjFunction* function)
		{
			ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*, function->upvalueCount);
			for (int i = 0; i < function->upvalueCount; i++)
			{
				upvalues[i] = nullptr;
			}

			ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
			closure->function = function;
			closure->upvalues = upvalues;
			closure->upvalueCount = function->upvalueCount;
			return closure;
		}

		ObjClass* newClass(ObjString* name)
		{
			ObjClass* cls = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
			cls->name = name;
			initTable(&cls->methods);
			return cls;
		}

		ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method)
		{
			ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod, OBJ_BOUND_METHOD);
			bound->receiver = receiver;
			bound->method = method;
			return bound;
		}

		ObjInstance* newInstance(ObjClass* cls)
		{
			ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
			instance->cls = cls;
			initTable(&instance->fields);
			return instance;
		}

		ObjUpvalue* newUpvalue(Value* slot)
		{
			ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);
			upvalue->location = slot;
			upvalue->closed = NIL_VAL;
			upvalue->next = nullptr;
			return upvalue;
		}

		ObjNative* newNative(NativeFn function)
		{
			ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
			native->function = function;
			return native;
		}

		uint32_t hashString(const char* key, int length)
		{
			uint32_t hash = 2166136261u;
			for (int i = 0; i < length; ++i)
			{
				hash ^= key[i];
				hash *= 16777619;
			}
			return hash;
		}
		ObjString* allocateString(char* chars, int length, uint32_t hash)
		{
			ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
			string->length = length;
			string->chars = chars;
			string->hash = hash;
			
			push(OBJ_VAL(string));
			tableSet(&vm.strings, string, NIL_VAL);
			pop();

			return string;
		}

		ObjString* takeString(char* chars, int length)
		{
			uint32_t hash = hashString(chars, length);
			ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
			if (interned != nullptr)
			{
				FREE_ARRAY(char, chars, length + 1);
				return interned;
			}
			return allocateString(chars, length, hash);
		}

		ObjString* copyString(const char* chars, int length)
		{
			uint32_t hash = hashString(chars, length);
			ObjString* interned = tableFindString(&vm.strings, chars, length, hash);
			if (interned != nullptr) return interned;

			char* heapChars = ALLOCATE(char, length + 1);
			memcpy(heapChars, chars, length);
			heapChars[length] = '\0';

			return allocateString(heapChars, length, hash);
		}

		void printFunction(ObjFunction* function)
		{
			if (function->name == nullptr)
			{
				printf("<script>");
				return;
			}
			printf("<fn %s>", function->name->chars);
		}

		void printObject(Value value)
		{
			switch (OBJ_TYPE(value))
			{
			case OBJ_CLASS:
			{
				printf("%s", AS_CLASS(value)->name->chars);
				break;
			}
			case OBJ_BOUND_METHOD:
			{
				printFunction(AS_BOUND_METHOD(value)->method->function);
				break;
			}
			case OBJ_INSTANCE:
			{
				printf("%s instance", AS_INSTANCE(value)->cls->name->chars);
				break;
			}
			case OBJ_FUNCTION:
			{
				printFunction(AS_FUNCTION(value));
				break;
			}
			case OBJ_CLOSURE:
			{
				printFunction(AS_CLOSURE(value)->function);
				break;
			}
			case OBJ_UPVALUE:
			{
				printf("upvalue");
				break;
			}
			case OBJ_NATIVE:
			{
				printf("<native fn>");
				break;
			}
			case OBJ_STRING:
				printf("%s", AS_CSTRING(value));
				break;
			}
		}
}
}