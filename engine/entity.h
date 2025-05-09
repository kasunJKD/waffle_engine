#ifndef ENTITY_H
#define ENTITY_H
#include "glm/fwd.hpp"
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
    TEXT
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
    float scale;
    glm::vec3 color;

    //fontmanger manages font renders no need these
    const char* texture_name;
    const char* shader_name;
    GLuint VAO;

    entityId parent_id;
    std::vector<Entity*> children;

    Sprite *sprite;

    bool active;

    //collisions
 glm::vec2 halfSize;       // half‐width/height of the box
  glm::vec2 colliderOffset; // offset from sprite top‐left to box top‐left
  bool      collidable;
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
GLuint spriteVAO;
    GLuint spriteVBO;
    GLuint spriteEBO;
};

struct EntitySystem {
    Entity entities[MAX_ENTITIES];
    size_t entity_count;
};

void entity_system_init(Temp_Allocator::TempArena* arena);
Entity* create_entity(const char* name);
Entity* create_entity(const char* name, EntityType eType, RenderSystem* r);
void remove_entity(entityId id);
EntitySystem *get_entity_manager();
#endif //ENTITY_H
