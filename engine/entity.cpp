#include "entity.h"
#include "allocator.h"
#include "debug.h"
#include <cstring>

static EntityList* entity_list = nullptr;

// Global entity counter.
static entityId g_nextEntityId = 1;

void entity_system_init(MEM::MemoryArena *arena)
{
  if(entity_list == nullptr)
  {
    entity_list = static_cast<EntityList*>(MEM::arena_alloc(arena, sizeof(EntityList)));
    entity_list->entity_count = 0;
    std::memset(entity_list, 0, sizeof(EntityList));
  }
  else {
    DEBUG_ERROR("entity system is already initialised");
    DEBUG_ASSERT(entity_list != nullptr);
  }
}

Entity* create_entity(MEM::MemoryArena* arena, const char* name) {
    if (entity_list->entity_count >= MAX_ENTITIES) return nullptr;

    Entity* entity = static_cast<Entity*>(MEM::arena_alloc(arena, sizeof(Entity)));
    if (!entity) return nullptr;

    entity->id = g_nextEntityId++;
    entity->name = name;
    entity->component_count = 0;

    entity_list->entities[entity_list->entity_count++] = entity;
    DEBUG_LOG("Created entity with ID %d", entity->id);
    return entity;
}

VelocityComponent* add_VelocityComponent(MEM::MemoryArena* arena, Entity* entity)
{

    DEBUG_ASSERT(entity->component_count <= COMPONENT_LIMIT);

    VelocityComponent* component = static_cast<VelocityComponent*>(MEM::arena_alloc(arena, sizeof(VelocityComponent)));
    if (!component) return nullptr;

    entity->components[entity->component_count++] = component;
    return component;
}

//@TODO have to implement remove chunk of entities 
//@TODO bundle components to gether keep array of components of each type
