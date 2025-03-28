#include "entity.h"
#include "allocator.h"
#include "debug.h"
#include "glrenderSystem.h"
#include <cassert>
static EntitySystem* entity_system = nullptr;
static entityId g_nextEntityId = 1;

void entity_system_init(Temp_Allocator::TempArena* arena) {
    if (entity_system) {
        DEBUG_ERROR("Entity system is already initialized");
        return;
    }

    void* memory = Temp_Allocator::temp_arena_alloc(
        arena, sizeof(EntitySystem));

    if (!memory) {
        DEBUG_ERROR("Failed to allocate memory for EntitySystem!");
        return;
    }

    entity_system = new (memory) EntitySystem(); // Placement new
    entity_system->entity_count = 0;

    DEBUG_LOG("Entity system initialized successfully");
}

Entity* create_entity(const char* name) {
    if (!entity_system || entity_system->entity_count >= MAX_ENTITIES) return nullptr;

    Entity* entity = &entity_system->entities[entity_system->entity_count++];
    entity->id = g_nextEntityId++;
    entity->name = name;

    DEBUG_LOG("Created entity with ID %d", entity->id);
    return entity;
}
Entity* create_entity(const char* name, EntityType eType, RenderSystem* r)
{
    if (!entity_system || entity_system->entity_count >= MAX_ENTITIES) return nullptr;

    Entity* entity = &entity_system->entities[entity_system->entity_count++];
    entity->id = g_nextEntityId++;
    entity->name = name;

    switch (eType) {
        case WORLD: {
            createRenderComponent(r, entity);
            break;
        };
        default:
            break;
    }

    DEBUG_LOG("Created entity with ID %d", entity->id);
    return entity;
    
}

void remove_entity(entityId id) {
    if (!entity_system) return;

    for (size_t i = 0; i < entity_system->entity_count; ++i) {
        if (entity_system->entities[i].id == id) {
            entity_system->entities[i] = entity_system->entities[--entity_system->entity_count];
            DEBUG_LOG("Removed entity with ID %d", id);
            return;
        }
    }
}
