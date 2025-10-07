#ifndef ENTITY_H 
#define ENTITY_H

#include "allocator.h"
#include "defines.h"
#include <cstdint>

enum Type {
    T_TEST
};

enum Flag {
    F_RENDER = 1 << 0
};

struct Entity {
    uint32_t id;
    Type type;
    Flag flag;

    vec3 position;
    bool active;
};


struct EntityManager {
    Entity* entities;
    size_t capacity; //total slots
    size_t count; //inuse amount
};

EntityManager create_entity_manager(size_t capacity, Arena* arena);

size_t add_entity(EntityManager* manager);
Entity* get_entity(EntityManager* manager, size_t id);


#endif
