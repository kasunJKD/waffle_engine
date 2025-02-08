#include "engine.h" // IWYU pragma: keep

#ifdef DEBUG_ENABLED
#include "debug.h"
#endif

#include <cstdio>
#include <cstdlib>

//TODO add systems Engine side audio, input etc 
//game side systems too
//shaders
//game logic code seperation
//game side components

int main() {
    size_t arenaSize = 1024 * 1024; // For example, 1 MB.
    MEM::MemoryArena* arena = MEM::arena_create(arenaSize);
    if (!arena) {
        DEBUG_ERROR("Failed to create MemoryArena.");
        return 1;
    }
    // Initialize engine and game component storage from the arena.
    Entity::initialize_component_storage(arena, arenaSize);
    
    Entity::Entity e = Entity::create_entity();
    
    Entity::TransformComponent t;
    t.position = glm::vec3(0.0f, 0.0f, 0.0f);
    t.rotation = 0.0f;
    t.scale = glm::vec2(1.0f, 1.0f);
    t.forward = glm::vec3(0.0f, 0.0f, 1.0f);
    t.up = glm::vec3(0.0f, 1.0f, 0.0f);
    Entity::add_transform_component(e, t);
    
    MEM::arena_print_stats(arena);

    // In a real application, you might reset the arena instead of destroying it.
    free(arena->base);
    free(arena);
    
    return 0;
}
