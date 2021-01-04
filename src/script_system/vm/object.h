#pragma once

#include "vm/common.h"
#include "vm/value.h"
#include "vm/chunk.h"
#include "vm/table.h"

namespace script_system {
namespace vm {

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_FUNCTION(value) isObjType(value, OBJ_FUNCTION)
#define IS_CLOSURE(value) isObjType(value, OBJ_CLOSURE)
#define IS_NATIVE(value) isObjType(value, OBJ_NATIVE)
#define IS_STRING(value) isObjType(value, OBJ_STRING)
#define IS_CLASS(value) isObjType(value, OBJ_CLASS)
#define IS_INSTANCE(value) isObjType(value, OBJ_INSTANCE)
#define IS_BOUND_METHOD(value) isObjType(value, OBJ_BOUND_METHOD)

#define AS_FUNCTION(value) ((ObjFunction*)AS_OBJ(value))
#define AS_CLOSURE(value) ((ObjClosure*) AS_OBJ(value))
#define AS_NATIVE(value) (((ObjNative*)AS_OBJ(value))->function)
#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)
#define AS_CLASS(value) ((ObjClass*)AS_OBJ(value))
#define AS_INSTANCE(value) ((ObjInstance*)AS_OBJ(value))
#define AS_BOUND_METHOD(value) ((ObjBoundMethod*)AS_OBJ(value))

	typedef enum {
		OBJ_CLASS,
		OBJ_BOUND_METHOD,
		OBJ_INSTANCE,
		OBJ_FUNCTION,
		OBJ_CLOSURE,
		OBJ_UPVALUE,
		OBJ_NATIVE,
		OBJ_STRING,
	} ObjType;

	struct sObj
	{
		ObjType type;
		bool isMarked;
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

	typedef struct {
		Obj obj;
		ObjString* name;
		Table methods;
	} ObjClass;
	ObjClass* newClass(ObjString* name);

	typedef struct {
		Obj obj;
		Value receiver;
		ObjClosure* method;
	} ObjBoundMethod;
	ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method);

	typedef struct {
		Obj obj;
		ObjClass* cls;
		Table fields;
	} ObjInstance;
	ObjInstance* newInstance(ObjClass* cls);


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