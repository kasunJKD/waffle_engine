#ifndef HASHTABLE_H 
#define HASHTABLE_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../defines.h"
#include "../allocator.h"

#define CAPACITY 5000

struct hitem {
    const char* key;
    Resource* data;
};

// No collision resolution like buckets implemented
struct hashTable {
    hitem** items;
    size_t count;
    size_t size;
    
    Pool_Allocator::Pool* pool;

    unsigned long hash_function(const char* str) {
        unsigned long i = 0;
        for (int j = 0; str[j]; j++)
            i += str[j];
        return i % CAPACITY;
    }

    hitem* createItem(const char* key, Resource* data) {
        hitem* item = (hitem*)Pool_Allocator::pool_alloc(pool);
        item->key = key;
        item->data = data;
        return item;
    }

hashTable* createTable(Pool_Allocator::Pool* pool_ref) {
    if (!pool_ref) {
        printf("Error: Pool is NULL. Cannot create hashTable.\n");
        return nullptr;
    }

    hashTable* table = (hashTable*)Pool_Allocator::pool_alloc(pool_ref);
    if (!table) {
        printf("Error: Failed to allocate hashTable.\n");
        return nullptr;
    }

    table->size = CAPACITY;
    table->count = 0;
    table->pool = pool_ref;

    // Allocate large array from heap
    table->items = (hitem**)malloc(sizeof(hitem*) * CAPACITY);
    if (!table->items) {
        printf("Error: Failed to allocate hashTable items array.\n");
        return nullptr;
    }

    memset(table->items, 0, sizeof(hitem*) * CAPACITY);
    return table;
}

    bool checkKeyExists(const char* key) {
        for (size_t i = 0; i < size; i++) {
            if (items[i] && strcmp(items[i]->key, key) == 0) {
                return true;
            }
        }
        return false;
    }

    void print_table() {
        printf("\nHash Table\n-------------------\n");
        for (size_t i = 0; i < size; i++) {
            if (items[i]) {
                printf("Index:%zu, Key:%s\n", i, items[i]->key);
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

        while (items[index] != nullptr && strcmp(items[index]->key, key) != 0) {
            index = (index + 1) % size;
        }

        if (items[index] != nullptr) {
            items[index]->data = value;
            return;
        }

        hitem* item = createItem(key, value);
        items[index] = item;
        count++;
    }
    
    Resource* ht_search(const char* key) {
        size_t index = hash_function(key);
        size_t start_index = index;

        while (items[index] != nullptr) {
            if (strcmp(items[index]->key, key) == 0)
                return items[index]->data;

            index = (index + 1) % size;
            if (index == start_index) break; // prevent infinite loop
        }
        return nullptr;
    }
};

#endif // HASHTABLE_H

