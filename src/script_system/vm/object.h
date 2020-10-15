#pragma once

#include "vm/common.h"
#include "vm/value.h"

namespace script_system {
namespace vm {

#define OBJ_TYPE(value) (AS_OBJ(value)->type)
#define IS_STRING(value) isObjType(value, OBJ_STRING)

#define AS_STRING(value) ((ObjString*)AS_OBJ(value))
#define AS_CSTRING(value) (((ObjString*)AS_OBJ(value))->chars)

	typedef enum {
		OBJ_STRING,
	} ObjType;

	struct sObj
	{
		ObjType type;
		struct  sObj* next;
	};

	struct sObjString
	{
		Obj obj;
		int length;
		char* chars;
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