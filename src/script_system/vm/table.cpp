#include "vm/table.h"
#include "vm/memory.h"
#include "vm/value.h"
#include "vm/object.h"

namespace script_system {
namespace vm {

#define TABLE_MAX_LOAD 0.75

	Entry* findEntry(Entry* entries, int capacity, ObjString* key)
	{
		uint32_t index = key->hash % capacity;
		for (;;)
		{
			Entry* entry = &entries[index];
			if (entry->key == key || entry->key == nullptr)
			{
				return entry;
			}

			index = (index + 1) % capacity;
		}
	}

	void adjustCapacity(Table* table, int capacity)
	{
		Entry* entries = ALLOCATE(Entry, capacity);
		for (int i = 0; i < capacity; ++i)
		{
			entries[i].key = nullptr;
			entries[i].value = nullptr;
		}

		for (int i = 0; i < table->capacity; ++i)
		{
			Entry* entry = &table->entries[i];
			if (entry->key == nullptr) continue;

			Entry* dest = findEntry(entries, capacity, entry->key);
			dest->key = entry->key;
			dest->value = entry->value;
		}
		FREE_ARRAY(Entry, table->entries, table->capacity);

		table->entries = entries;
		table->capacity = capacity;
	}

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

	bool tableSet(Table* table, ObjString* key, Value value)
	{
		if ((table->count + 1) > table->capacity * TABLE_MAX_LOAD)
		{ 
			int capacity = GROW_CAPACITY(table->capacity);
			adjustCapacity(table, capacity);
		}
		Entry* entry = findEntry(table->entries, table->capacity, key);

		const bool isNewKey = entry->key == nullptr;
		if (isNewKey) table->count++;

		entry->key = key;
		entry->value = value;

		return isNewKey;
	}

	void tableAddAll(Table* from, Table* to)
	{
		for (int i = 0; i < from->capacity; ++i)
		{
			Entry* entry = &from->entries[i];
			if (entry->key != nullptr)
			{
				tableSet(to, entry->key, entry->value);
			}
		}
	}

	bool tableGet(Table* table, ObjString* key, Value* value)
	{
		if (table->count == 0) return false;

		Entry* entry = findEntry(table->entries, table->capacity, key);
		if (entry->key == nullptr) return false;

		*value = entry->value;		
		return true;
	}

	bool tableDelete(Table* table, ObjString* key)
	{
		if (table->count == 0) return false;
		
		Entry* entry = findEntry(table->entries, table->capacity, key);
		if (entry->key == nullptr) return false;

		entry->key = nullptr;
		entry->value = BOOL_VAL(true);

		return true;
		return true;
	}

}
}