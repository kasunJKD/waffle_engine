#include "engine.h" // IWYU pragma: keep
#include "resourceManager.h"

#ifdef DEBUG_ENABLED
#include "debug.h"
#endif

#include <cstdio>
#include <cstdlib>

//TODO add systems Engine side audio, input etc 
//game side systems too
//shaders
//game logic code seperation
//game side components

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; // Time per frame in milliseconds

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}


int main() {
    Window window;
    InputManager inputManager;
    
    // Initialize the window
    if (!window.init("Sandbox", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
        DEBUG_ERROR("Failed to initialize window.");
        return -1;
    }
    
    size_t arenaSize = 1024 * 1024; // For example, 1 MB.
    MEM::MemoryArena* arena = MEM::arena_create(arenaSize);
    if (!arena) {
        DEBUG_ERROR("Failed to create MemoryArena.");
        return 1;
    }

    ResourceManager* r_manager = createResourceManager(arenaSize);
    Resource* spritesheet = load(r_manager, "assets/tiled/testSpritesheet.png", TEXTURE);
    if (!spritesheet)
    {
        DEBUG_ERROR("spritesheet error");
        return -1;
    }
    
    // Initialize engine and game component storage from the arena.
    Entity::initialize_component_storage(arena, arenaSize);
    
    Entity::Entity e = Entity::create_entity();
    
    Entity::TransformComponent t;
    t.position = glm::vec3(0.0f, 0.0f, 0.0f);
    t.rotation = 0.0f;
    t.scale = glm::vec2(1.0f, 1.0f);
    t.forward = glm::vec3(0.0f, 0.0f, 1.0f);
    t.up = glm::vec3(0.0f, 1.0f, 0.0f);
    Entity::add_transform_component(e, t);

inputManager.init();

    bool isRunning = true;
    while (isRunning) {
         uint32_t frameStart = SDL_GetTicks();
        //float dt = calculateDeltaTime();
        
inputManager.update(isRunning);
        
         // Clear the screen
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        window.swapBuffers();

// Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }
    
    MEM::arena_print_stats(arena);

    // In a real application, you might reset the arena instead of destroying it.
    free(arena->base);
    free(arena);
    destroyResourceManager(r_manager);
    window.cleanUp();
    return 0;
}
