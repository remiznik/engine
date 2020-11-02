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

			object->next = vm.objects;
			vm.objects = object;

			return object;
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
			
			tableSet(&vm.strings, string, NIL_VAL);

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



		void printObject(Value value)
		{
			switch (OBJ_TYPE(value))
			{
			case OBJ_STRING:
				printf("%s", AS_CSTRING(value));
				break;
			}
		}
}
}