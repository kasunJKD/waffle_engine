// globals.h
#ifndef GLOBALS_H
#define GLOBALS_H

#include "allocator.h"
#include "assetsManager.h"
#include "camera.h"
#include "editor.h"
#include "entity.h"
#include "glrenderer.h"
#include "input_s.h"
#include "renderer.h"
#include "window.h"

struct State {
    Window* window; 
    InputManager* inputManager;
    Camera* camera;
    AssetManager* asset_manager; //keep assets manager in a seperate arena and store a pointer to it
    EntityManager* entity_manager;
    RenderSystem* render_system;
    GLRENDER* gl_renderer;
    Arena* arena;
    Arena* scratch_buffer;
    Arena* asset_arena;

    #ifdef DEBUG_ENABLED
    Editor* editor;
    #endif
    
    bool isRunning;
    bool isDebug;
};

extern State* state; // extern global pointer

#endif
