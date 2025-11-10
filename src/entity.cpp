#include "entity.h"
#include "allocator.h"
#include <cassert>
#include <cstddef>

size_t add_entity(EntityManager* manager)
{
    assert(manager->count < manager ->capacity);

    size_t id = manager->count++;
    return id;
}

Entity* get_entity(EntityManager* manager, size_t id)
{
    assert(id < manager->count);
    return &manager->entities[id];
}

EntityManager* create_entity_manager(size_t capacity, Arena* arena)
{
    EntityManager *manager = PushStruct(arena, EntityManager);
    manager->count = 0;
    manager->capacity = capacity;
    manager->entities = PushArrayZero(arena, Entity, capacity);
    return manager;
}
