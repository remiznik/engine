#pragma once

#include "vm/value.h"

namespace script_system {
namespace vm {

typedef struct {
	ObjString* key;
	Value value;
} Entry;

typedef struct {
	int count;
	int capacity;
	Entry* entries;
} Table;

void initTable(Table* table);
void freeTable(Table* table);	
}
}