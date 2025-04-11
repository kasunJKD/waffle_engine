#include "hashTable.h"
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
