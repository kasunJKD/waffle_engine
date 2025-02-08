#include "entity.h"
#include "allocator.h"
#include "debug.h"
#include <cstdio>
#include <cstring>

static MEM::MemoryArena *g_arena = nullptr;

namespace Entity {
// This function should be called once by the engine during initialization,
// Global arrays for built-in component storage allocated from the arena.
static TransformComponent *g_transformComponents = nullptr;
static bool *g_transformComponentPresent = nullptr;

static VelocityComponent *g_velocityComponents = nullptr;
static bool *g_velocityComponentPresent = nullptr;

// Global entity counter.
static EntityId g_nextEntityId = 1;

// passing in an arena that was created with arena_create().
void initialize_component_storage(void *arenaMemory, size_t arenaSize) {
  g_arena = (MEM::MemoryArena *)arenaMemory;

  // Calculate required memory.
  size_t requiredMemory =
      (sizeof(TransformComponent) + sizeof(bool)) * MAX_ENTITIES +
      (sizeof(VelocityComponent) + sizeof(bool)) * MAX_ENTITIES;

  if (arenaSize < requiredMemory) {
    DEBUG_ERROR(
        "Not enough memory in arena: required %zu bytes, provided %zu bytes.",
        requiredMemory, arenaSize);
    return;
  }

  g_transformComponents = (TransformComponent *)arena_alloc(
      g_arena, sizeof(TransformComponent) * MAX_ENTITIES);

  g_transformComponentPresent =
      (bool *)arena_alloc(g_arena, sizeof(bool) * MAX_ENTITIES);
  memset(g_transformComponentPresent, 0, sizeof(bool) * MAX_ENTITIES);

  g_velocityComponents = (VelocityComponent *)arena_alloc(
      g_arena, sizeof(VelocityComponent) * MAX_ENTITIES);

  g_velocityComponentPresent =
      (bool *)arena_alloc(g_arena, sizeof(bool) * MAX_ENTITIES);
  memset(g_velocityComponentPresent, 0, sizeof(bool) * MAX_ENTITIES);

  DEBUG_LOG("Engine component storage initialized", MAX_ENTITIES);
}

Entity create_entity() {
  Entity e;
  e.id = g_nextEntityId++;
  DEBUG_LOG("Created entity with ID %d", e.id);
  return e;
}

void add_transform_component(Entity e, const TransformComponent &transform) {
  if (e.id >= MAX_ENTITIES) {
    DEBUG_ERROR("Entity id out of range in add_transform_component");
    return;
  }
  g_transformComponents[e.id] = transform;
  g_transformComponentPresent[e.id] = true;
}

void add_velocity_component(Entity e, const VelocityComponent &velocity) {
  if (e.id >= MAX_ENTITIES) {
    DEBUG_ERROR("Entity id out of range in add_velocity_component");
    return;
  }
  g_velocityComponents[e.id] = velocity;
  g_velocityComponentPresent[e.id] = true;
}

bool has_transform_component(Entity e) {
  if (e.id >= MAX_ENTITIES)
    return false;
  return g_transformComponentPresent[e.id];
}

bool has_velocity_component(Entity e) {
  if (e.id >= MAX_ENTITIES)
    return false;
  return g_velocityComponentPresent[e.id];
}
}
