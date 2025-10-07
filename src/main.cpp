#include <cstdint>

#include "allocator.h"
#include "assetsManager.h"
#include "camera.h"
#include "editor.h"
#include "entity.h"
#include "glrenderer.h"
#include "input_s.h"
#include "renderer.h"
#include "shader.h"
#include "texture.h"
#include "window.h"
#include "debug.h"

#define MAX_ENTITIES 100

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; 

constexpr uint64_t ARENA_MEMORY_MB = 20;
constexpr uint64_t ARENA_MEMORY_MB_ASSETS = 50;

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

struct State {
    Window window; 
    InputManager inputManager;
    Camera camera;
    AssetManager* asset_manager; //keep assets manager in a seperate arena and store a pointer to it
    EntityManager entity_manager;
    RenderSystem render_system;
    GLRENDER gl_renderer;

    #ifdef DEBUG_ENABLED
    Editor editor;
    #endif
    
    bool isRunning;
    bool isDebug;
};

State* state = {0};

void test_create_entities(EntityManager* e_manager){
    size_t entity_id = add_entity(e_manager);
    Entity* entity = get_entity(e_manager, entity_id);
    entity->flag = F_RENDER;
    entity->type = T_TEST;
    entity->position = vec3(0.0f, 0.0f, 0.0f);
    entity->active = true;
}

void init() {
    Arena* arena = ArenaAllocateMB(ARENA_MEMORY_MB);
    Arena* asset_arena = ArenaAllocateMB(ARENA_MEMORY_MB_ASSETS);

    state = PushStruct(arena, State);

    if (!state->window.init("Game", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
	DEBUG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    size_t number_of_shaders = 5;
    size_t number_of_textures = 5;
    size_t number_of_meshes = 5;


    AssetManager* asset_manager = PushStruct(asset_arena, AssetManager);
    *asset_manager = AssetManager_Create(asset_arena, number_of_meshes + number_of_shaders + number_of_textures);
    add_shader(asset_arena, asset_manager, "D:\\Personal\\waffle_engine\\shaders\\test.vert", "D:\\Personal\\waffle_engine\\shaders\\test.frag", "test_shader", TEXT_SHADER);
    add_texture(asset_manager, "D:\\Personal\\waffle_engine\\assets\\container.jpg", "image_container", TEST_TEXTURE);


    state->inputManager.init();
    state->asset_manager = asset_manager;
    state->camera = createCamera();
    state->entity_manager = create_entity_manager(100, arena);

    gl_render_init(&state->gl_renderer);

    create_render_system(&state->render_system,  &state->entity_manager, state->asset_manager, &state->gl_renderer);

    Editor::activate_editor(&state->editor);
    
    state->isRunning = true;

    test_create_entities(&state->entity_manager);
}

void update_game() {
        uint32_t frameStart = SDL_GetTicks();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }

        #ifdef DEBUG_ENABLED
            if(state->editor.active) {
                state->editor.update();
            }
        #endif


        update_render_system(&state->render_system);
        state->window.swapBuffers();
        
}

void process_input() {
        state->inputManager.update(state->isRunning, &state->window);
}

void deinit(){
    state->window.cleanUp();
}

int main() {
    init();

    while (state->isRunning) {
        // float dt = calculateDeltaTime();
        // process_input(dt);
	process_input();
        update_game();
    }
    
    deinit();

    return 0;
}
