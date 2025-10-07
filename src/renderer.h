#ifndef RENDERER_H
#define RENDERER_H

//stack of commands
//push 

#include "assetsManager.h"
#include "entity.h"
#include "glrenderer.h"

struct RenderSystem {
    EntityManager* entityManager;
    AssetManager* assetManager;
    GLRENDER* glRenderer;
};

void create_render_system(RenderSystem* renderSystem, EntityManager* entityManager, AssetManager* assetManager_, GLRENDER* glRenderer_);
void update_render_system(RenderSystem* renderSystem);


// render -> get entities with rendables => for each type call gl draw with entitiy

#endif
