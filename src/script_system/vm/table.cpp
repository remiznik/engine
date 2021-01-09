#include "vm/table.h"
#include "vm/memory.h"
#include "vm/value.h"
#include "vm/object.h"

#include "core/types.h"

namespace script_system {
namespace vm {

#define TABLE_MAX_LOAD 0.75

	Entry* findEntry(Entry* entries, int capacity, ObjString* key)
	{
		uint32_t index = key->hash & capacity;
		Entry* tombstone = nullptr;
		for (;;)
		{
			Entry* entry = &entries[index];
			if (entry->key == nullptr)
			{
				if (IS_NIL(entry->value))
				{
					return tombstone != nullptr ? tombstone : entry;
				} 
				else
				{
					if (tombstone == nullptr)
					{
						tombstone = entry;
					}
				}
			}
			else if (entry->key == key)
			{
				return entry;
			}

			index = (index + 1) & capacity;
		}
	}

	void adjustCapacity(Table* table, int capacity)
	{
		Entry* entries = ALLOCATE(Entry, capacity + 1);
		for (int i = 0; i <= capacity; ++i)
		{
			entries[i].key = nullptr;
			entries[i].value = NIL_VAL;
		}
		
		for (int i = 0; i <= table->capacity; ++i)
		{
			Entry* entry = &table->entries[i];
			if (entry->key == nullptr) continue;

			Entry* dest = findEntry(entries, capacity, entry->key);
			dest->key = entry->key;
			dest->value = entry->value;
			table->count++;
		}
		FREE_ARRAY(Entry, table->entries, table->capacity + 1);

		table->entries = entries;
		table->capacity = capacity;
	}

	void initTable(Table* table)
	{
		table->count = 0;
		table->capacity = -1;
		table->entries = nullptr;
	}
	
	void freeTable(Table* table)
	{
		FREE_ARRAY(Entry, table->entries, table->capacity);
		initTable(table);
	}

	bool tableSet(Table* table, ObjString* key, Value value)
	{
		if ((table->count + 1) > (table->capacity + 1)* TABLE_MAX_LOAD)
		{ 
			int capacity = GROW_CAPACITY(table->capacity + 1) - 1;
			adjustCapacity(table, capacity);
		}
		Entry* entry = findEntry(table->entries, table->capacity, key);

		const bool isNewKey = entry->key == nullptr;
		if (isNewKey && IS_NIL(entry->value)) table->count++;

		entry->key = key;
		entry->value = value;

		return isNewKey;
	}

	void tableAddAll(Table* from, Table* to)
	{
		for (int i = 0; i <= from->capacity; ++i)
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

	ObjString* tableFindString(Table* table, const char* chars, int length, uint32_t hash)
	{
		if (table->count == 0) return nullptr;

		uint32_t index = hash & table->capacity;

		for (;;)
		{
			Entry* entry = &table->entries[index];
			if (entry->key == nullptr)
			{
				if (IS_NIL(entry->value)) return nullptr;
			}
			else if (entry->key->length == length && entry->key->hash == hash && memcmp(entry->key->chars, chars, length) == 0)
			{
				return entry->key;
			}

			index = (index + 1) & table->capacity;
		}

		return nullptr;
	}

	void markTable(Table* table)
	{
		for (int i = 0; i < table->capacity; i++)
		{
			Entry* entry = &table->entries[i];
			memory::markObject((Obj*)entry->key);
			memory::markValue(entry->value);
		}
	}

	void tableRemoveWhite(Table* table)
	{
		for (int i = 0; i < table->capacity; ++i)
		{
			Entry* entry = &table->entries[i];
			if (entry->key != nullptr && !entry->key->obj.isMarked)
			{
				tableDelete(table, entry->key);
			}
		}
	}


}
}