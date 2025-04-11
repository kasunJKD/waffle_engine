#ifndef HASHTABLE_H 
#define HASHTABLE_H

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <variant>
#include "../defines.h"
#include "../allocator.h"

#define CAPACITY 5000

struct hitem {
    const char* key;
    std::variant<Sprite*, Resource*> data;
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
    hitem* createItem(const char* key, Sprite* data) {
        hitem* item = (hitem*)Pool_Allocator::pool_alloc(pool);
        item->key = key;
        item->data = data;
        return item;
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
    void ht_insert(const char* key, Sprite* value) {
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
    
Resource* ht_search_resource(const char* key) {
    size_t index = hash_function(key);
    size_t start_index = index;

    while (items[index] != nullptr) {
        if (strcmp(items[index]->key, key) == 0) {
            // Check if the stored value is a Resource*
            if (std::holds_alternative<Resource*>(items[index]->data)) {
                return std::get<Resource*>(items[index]->data);
            } else {
                return nullptr; // key exists but value is not a Resource*
            }
        }

        index = (index + 1) % size;
        if (index == start_index) break;
    }

    return nullptr;
}
Sprite* ht_search_sprite(const char* key) {
    size_t index = hash_function(key);
    size_t start_index = index;

    while (items[index] != nullptr) {
        if (strcmp(items[index]->key, key) == 0) {
            if (std::holds_alternative<Sprite*>(items[index]->data)) {
                return std::get<Sprite*>(items[index]->data);
            } else {
                return nullptr;
            }
        }

        index = (index + 1) % size;
        if (index == start_index) break;
    }

    return nullptr;
}
};

hashTable* createTable(Pool_Allocator::Pool* pool_ref);
#endif // HASHTABLE_H

