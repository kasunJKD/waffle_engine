#ifndef ENTITY_H
#define ENTITY_H
#include "resourceManager.h"
#pragma once
#include <cstdint>
#include "allocator.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <vector>

#define MAX_ENTITIES 1024

using entityId = uint32_t;

enum EntityType {
    PLAYER,
    SOUND,
    ENEMY,
    WORLD,
    CAMERA,
    MONSTER,
};

enum WorldType {
    WORLD1,
    WORLD2
};

struct Entity {
    entityId id;
    EntityType type;
    WorldType worldType;
    
    const char* name;

    glm::vec3 position;

    const char* texture_name;
    const char* shader_name;
    GLuint VAO;

    entityId parent_id;
    std::vector<Entity*> children;

    bool active;
};

struct Camera: Entity {
    glm::mat4 projection;
    glm::mat4 view;
    float zoom;         // Camera zoom level
    float width;        // camera width = 512
    float height;       // camera height = 288
};

#define MAX_RENDER_COMPONENTS 128

struct RenderSystem {
    Entity* components[MAX_RENDER_COMPONENTS];
    int count;

    ResourceManager* resManager;
};

struct EntitySystem {
    Entity entities[MAX_ENTITIES];
    size_t entity_count;
};

void entity_system_init(MEM::MemoryArena* arena);
Entity* create_entity(const char* name);
Entity* create_entity(const char* name, EntityType eType, RenderSystem* r);
void remove_entity(entityId id);
#endif //ENTITY_H
