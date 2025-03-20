#ifndef HASHTABLE_H 
#define HASHTABLE_H

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "../defines.h"
#include "../allocator.h"

#define CAPACITY 5000

struct hitem {
    const char* key;  // now const because we don’t modify the key
    Resource* data;
};

//@haven't implemented collisions or buckets
struct hashTable {
    hitem** items;
    size_t count;
    size_t size;
    
    MEM::MemoryArena* arena;

    unsigned long hash_function(const char* str) {
        unsigned long i = 0;
        for (int j = 0; str[j]; j++)
            i += str[j];
        return i % CAPACITY;
    }

    hitem* createItem(const char* key, Resource* data) {
        hitem* item = (hitem*)MEM::arena_alloc(arena, sizeof(hitem));
        item->key = key;
        item->data = data;
        return item;
    }
    hashTable* createTable() {
        if (!arena) {
            printf("Error: Arena is NULL. Cannot create hashTable.\n");
            return nullptr;
        }

        // Allocate the hashTable itself from the arena
        hashTable* table = (hashTable*)MEM::arena_alloc(arena, sizeof(hashTable));
        if (!table) {
            printf("Error: Failed to allocate hashTable.\n");
            return nullptr;
        }

        table->size = CAPACITY;
        table->count = 0;
        table->arena = arena; // Propagate arena to table

        // Allocate array of pointers for hitem*
        table->items = (hitem**)MEM::arena_alloc(arena, sizeof(hitem*) * table->size);
        if (!table->items) {
            printf("Error: Failed to allocate hashTable items array.\n");
            return nullptr;
        }

        // Initialize all entries with empty `hitem` structs
        for (size_t i = 0; i < table->size; i++) {
            table->items[i] = nullptr;  // Ensure null pointers first
        }

        return table;
    }

    bool checkKeyExists(const char* key) {
        // Loop through the table; note that you need to check for null items.
        for (size_t i = 0; i < count; i++) {
            if (items[i] && strcmp(items[i]->key, key) == 0) {
                return true;
            }
        }
        return false;
    }

    void free_item(hitem* item) {
        // If keys are not allocated (i.e. they come from a literal or managed storage),
        // do NOT free them. Uncomment the following if you know the key is allocated:
        // free((void*)item->key);
        free(item->data);
        free(item);
    }

    void free_table(hashTable* table) {
        for (size_t i = 0; i < table->size; i++) {
            hitem* item = table->items[i];
            if (item != NULL)
                free_item(item);
        }
        free(table->items);
        free(table);
    }

    void print_table(hashTable* table) {
        printf("\nHash Table\n-------------------\n");
        for (size_t i = 0; i < table->size; i++) {
            if (table->items[i]) {
                printf("Index:%zu, Key:%s\n", i, table->items[i]->key);
            }
        }
        printf("-------------------\n\n");
    }

    void ht_insert(const char* key, Resource* value) {
        if (count == size) {
            printf("Insert Error: Hash Table is full\n");
            return;
        }

        size_t index = hash_function(key);

        // Linear probing to find an available slot
        while (items[index] != nullptr && strcmp(items[index]->key, key) != 0) {
            index = (index + 1) % size;  // Move to the next index (circular)
        }

        // If updating an existing key, just change the value
        if (items[index] != nullptr) {
            items[index]->data = value;
            return;
        }

        // Create a new item and insert it
        hitem* item = createItem(key, value);
        items[index] = item;
        count++;
    }
    
    Resource* ht_search(const char* key) {
        size_t index = hash_function(key);
        hitem* item = items[index];
        if (item != NULL && strcmp(item->key, key) == 0)
            return item->data;
        return nullptr;
    }
};



#endif //HASHTABLE_H
