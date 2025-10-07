#include "hashmap.h"
#include "allocator.h"
#include <cstddef>
#include <cstdint>

static uint32_t hash_str(const char* s)
{
    uint32_t h = 2166136261u;
    while (*s) { h ^= (unsigned char)(*s++); h *= 16777619u; }
    return h;
}

static size_t next_pow2(size_t v)
{
    v--; v |= v>>1; v |= v>>2; v |= v>>4; v |= v>>8; v |= v>>16; v++;
    return v < 4 ? 4 : v;
}

ht ht_create(Arena *a, size_t initialCap)
{
    size_t cap = next_pow2(initialCap);
    ht h;
    h.capacity = cap;
    h.length = 0;
    h.allocator = a;
    h.items = PushArrayZero(a, ht_item, cap);
    return h;
}

void ht_put(ht* h, const char* key, uintptr_t value)
{
    assert(h->length < h->capacity * 0.75);         /* resize not implemented */
    size_t mask = h->capacity - 1;
    uint32_t hcode = hash_str(key);

    while (1) {
        ht_item* slot = &h->items[hcode & mask];
        if (!slot->key) {
            /* duplicate key into arena so lifetime == table */
            size_t len = strlen(key)+1;
            char*  keyCopy = PushArray(h->allocator, char, len);
            memcpy(keyCopy, key, len);
            slot->key   = keyCopy;
            slot->value = value;
            h->length++;
            return;
        }
        else if (strcmp(slot->key, key) == 0) {     /* replace */
            slot->value = value;
            return;
        }
        ++hcode;
    }
}

uintptr_t ht_get(const ht* h, const char* key)
{
    if (!h->capacity) return NULL;
    size_t mask = h->capacity - 1;
    uint32_t hcode = hash_str(key);

    while (1) {
        const ht_item* slot = &h->items[hcode & mask];
        if (!slot->key)       return NULL;          /* empty stop-slot */
        if (strcmp(slot->key, key) == 0) return slot->value;
        ++hcode;
    }
}

void ht_clear(ht* h)
{
    memset(h->items, 0, h->capacity * sizeof(ht_item));
    h->length = 0;
}
