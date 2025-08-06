#ifndef ENTITY_H 
#define ENTITY_H

#include "defines.h"
#include <cstdint>

enum Type {
    TEST_ENTITY
};

enum Flag {
    ENTITY_FLAG_TEST = 1 << 0
};

struct Entity {
    uint32_t id;
    Type type;
    Flag flag;

    vec3 position;
};

struct EntityManager {
    Entity* entities;
    size_t capacity; //total slots
    size_t count; //inuse amount
};


#endif
