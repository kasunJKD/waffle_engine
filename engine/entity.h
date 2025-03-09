#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include "allocator.h"
#include "glad/glad.h"
#include <glm/glm.hpp>

#define MAX_ENTITIES 1024

using entityId = uint32_t;

enum EntityType {
    PLAYER,
    ENEMY
};

struct World {
    GLuint texture;
    GLuint fbo;
  // The texture used as the color attachment of the FBO:
    GLuint fboTexture;
    int8_t worldnumber;
};

struct Entity {
    entityId id;
    EntityType type;
    const char* name;

    glm::vec3 position;
    float rotation; 
    glm::vec2 scale;

    glm::vec3 velocity;

    int8_t world_belong_to;
    int8_t current_world;
};

struct EntitySystem {
    Entity entities[MAX_ENTITIES];
    size_t entity_count;
};

void entity_system_init(MEM::MemoryArena* arena);
Entity* create_entity(const char* name);
void remove_entity(entityId id);
#endif //ENTITY_H
