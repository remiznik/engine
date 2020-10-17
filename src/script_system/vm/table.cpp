#include "vm/table.h"
#include "vm/memory.h"
namespace script_system {
namespace vm {

void initTable(Table* table)
{
	table->count = 0;
	table->capacity = 0;
	table->entries = nullptr;
}

void freeTable(Table* table)
{
	FREE_ARRAY(Entry, table->entries, table->capacity);
	initTable(table);
}

}
}