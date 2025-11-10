#include "renderer.h"
#include "assetsManager.h"
#include "debug.h"
#include "entity.h"
#include "glrenderer.h"
#include <cstdint>

void create_render_system(RenderSystem* renderSystem, EntityManager* entityManager, AssetManager* assetManager_, GLRENDER* glRenderer_){
    renderSystem->entityManager = entityManager;
    renderSystem->assetManager = assetManager_;
    renderSystem->glRenderer = glRenderer_;
}

void render_frame(RenderSystem *r) {
    // glBindFramebuffer(GL_FRAMEBUFFER, r->gl->gameBuffer->fbo);
    // glViewport(0, 0, r->gl->gameBuffer->width, r->gl->gameBuffer->height);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (size_t i = 0; i < r->command_count; ++i) {
        RenderCommand *cmd = &r->drawCommands[i];

        switch (cmd->type) {
            // case RenderCommandType::Clear:
            //     glClearColor(cmd->clear.color.r, cmd->clear.color.g, cmd->clear.color.b, cmd->clear.color.a);
            //     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //     break;
            //
            // case RenderCommandType::Quad:
            //     draw_quad(cmd->quad.position, cmd->quad.size, cmd->quad.color);
            //     break;
            //
            // case RenderCommandType::Mesh:
            //     draw_mesh(cmd->mesh.mesh, cmd->mesh.texture, cmd->mesh.transform);
            //     break;

            case RenderCommandType::Texture:
                draw_texture(cmd->texture.position, cmd->texture.texture, cmd->texture.shader, cmd->texture.VAO);
                break;

            default:
                DEBUG_ERROR("wrong render frame type");
        }
    }

    r->command_count = 0; // reset
}


inline RenderCommand *push_command(RenderSystem *r) {
    assert(r->command_count < r->command_capacity);
    return &r->drawCommands[r->command_count++];
}


void push_texture_command(RenderSystem *r,uint32_t entity_id, vec3 pos, GLuint shader, GLuint tex, GLuint VAO) {
    RenderCommand *cmd = push_command(r);
    cmd->entity_id = entity_id;
    cmd->type = RenderCommandType::Texture;
    cmd->texture.position = pos;
    cmd->texture.texture = tex;
    cmd->texture.shader = shader;
    cmd->texture.VAO = VAO;
}
