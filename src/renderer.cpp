#include "renderer.h"
#include "assetsManager.h"
#include "entity.h"
#include "glrenderer.h"

void create_render_system(RenderSystem* renderSystem, EntityManager* entityManager, AssetManager* assetManager_, GLRENDER* glRenderer_){
    renderSystem->entityManager = entityManager;
    renderSystem->assetManager = assetManager_;
    renderSystem->glRenderer = glRenderer_;
}

void update_render_system(RenderSystem* renderSystem) {
    for(size_t i=0;renderSystem->entityManager->count > i; i++) {
        if(renderSystem->entityManager->entities[i].flag & F_RENDER) {
            gl_draw(&renderSystem->entityManager->entities[i], renderSystem->assetManager, renderSystem->glRenderer);
        }

    }
}
