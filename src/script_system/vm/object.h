#pragma once

#include "vm/common.h"
#include "vm/value.h"
#include "vm/chunk.h"

namespace script_system {
namespace vm {

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_FUNCTION(value) ((ObjFunction*)AS_OBJ(value))
#define AS_CLOSURE(value) ((ObjClosure*) AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative*)AS_OBJ(value))->function)
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

	typedef enum {
		OBJ_FUNCTION,
		OBJ_CLOSURE,
		OBJ_UPVALUE,
		OBJ_NATIVE,
		OBJ_STRING,
	} ObjType;

	struct sObj
	{
		ObjType type;
		struct  sObj* next;
	};

	typedef struct {
		Obj obj;
		int arity;
		int upvalueCount;
		Chunk chunk;
		ObjString* name;
	} ObjFunction;
	ObjFunction* newFunction();

	typedef struct ObjUpvalue {
		Obj obj;
		Value* location;
		Value closed;
		ObjUpvalue* next;
	} ObjUpvalue;
	ObjUpvalue* newUpvalue(Value* slot);

	typedef struct {
		Obj obj;
		ObjFunction* function;
		ObjUpvalue** upvalues;
		int upvalueCount;
	} ObjClosure;
	ObjClosure* newClosure(ObjFunction* function);


	typedef Value(*NativeFn)(int argCount, Value* args);
	typedef struct {
		Obj obj;
		NativeFn function;
	} ObjNative;
	ObjNative* newNative(NativeFn function);

	struct sObjString
	{
		Obj obj;
		int length;
		char* chars;
		uint32_t hash;
	};

	

	static inline bool isObjType(Value value, ObjType type)
	{
		return IS_OBJ(value) && AS_OBJ(value)->type == type;
	}

	ObjString* takeString(char* chars, int lenght);
	ObjString* copyString(const char* chars, int lenght);

	void printObject(Value value);
}
}