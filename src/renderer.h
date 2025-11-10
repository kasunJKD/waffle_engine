#ifndef RENDERER_H
#define RENDERER_H

//stack of commands
//push 

#include "assetsManager.h"
#include "entity.h"
#include "glrenderer.h"

enum class RenderCommandType {
    Sprite,
    Quad,
    Clear,
    Texture,
};

struct RenderCommand {
    RenderCommandType type;
    uint32_t sort_key;
    uint32_t entity_id;

    union {
        struct { vec3 position;} quad;
        struct { vec3 position;GLuint shader; GLuint texture; GLuint VAO;} texture;
        struct { } clear;
    };
};


struct RenderSystem {
    EntityManager* entityManager;
    AssetManager* assetManager;
    GLRENDER* glRenderer;

    Arena* frame_buffer;
    RenderCommand* drawCommands;
    size_t command_count;
    size_t command_capacity;
};

void create_render_system(RenderSystem* renderSystem, EntityManager* entityManager, AssetManager* assetManager_, GLRENDER* glRenderer_);

void push_texture_command(RenderSystem *r,uint32_t entity_id, vec3 pos, GLuint shader, GLuint tex, GLuint VAO);

void render_frame(RenderSystem *r);

#endif
