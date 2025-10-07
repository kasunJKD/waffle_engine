#ifndef HASHMAP_H
#define HASHMAP_H

//string to id hashmap
#include "allocator.h"
#include <cstddef>

struct ht_item {
    const char* key;
    uintptr_t value;
};

struct ht {
    ht_item* items;
    size_t capacity;
    size_t length;
    Arena* allocator;
};

ht     ht_create   (Arena* a, size_t initialCap);           /* cap rounded to pow-2 */
void   ht_put      (ht* h, const char* key, uintptr_t value);   /* key duplicated into arena */
 uintptr_t    ht_get      (const ht* h, const char* key);          /* NULL if not present */
void   ht_clear    (ht* h);                                 /* keeps capacity, resets len */

#endif
