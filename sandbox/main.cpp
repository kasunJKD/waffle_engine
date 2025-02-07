#include "entity.h"
#include "allocator.h"  // Your arena allocation functions.
#include "debug.h"
#include <cstdio>
#include <cstdlib>

//engine side engine.h engine.cpp API
//TODO add systems Engine side audio, input etc 
//game side systems too
//shaders
//game logic code seperation
//game side components
//add namespaces

int main() {
    size_t arenaSize = 1024 * 1024; // For example, 1 MB.
    MemoryArena* arena = arena_create(arenaSize);
    if (!arena) {
        DEBUG_ERROR("Failed to create MemoryArena.");
        return 1;
    }
    // Initialize engine and game component storage from the arena.
    initialize_component_storage(arena, arenaSize);
    
    Entity e = create_entity();
    
    TransformComponent t;
    t.position = glm::vec3(0.0f, 0.0f, 0.0f);
    t.rotation = 0.0f;
    t.scale = glm::vec2(1.0f, 1.0f);
    t.forward = glm::vec3(0.0f, 0.0f, 1.0f);
    t.up = glm::vec3(0.0f, 1.0f, 0.0f);
    add_transform_component(e, t);
    
    arena_print_stats(arena);

    // In a real application, you might reset the arena instead of destroying it.
    free(arena->base);
    free(arena);
    
    return 0;
}
