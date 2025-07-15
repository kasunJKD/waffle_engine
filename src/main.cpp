#include <cstdint>

#include "allocator.h"
#include "camera.h"
#include "editor.h"
#include "input_s.h"
#include "window.h"
#include "debug.h"

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; 

constexpr uint64_t ARENA_MEMORY_MB = 20;

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
    Editor editor;
    
    bool isRunning;
    bool isDebug;
};

State* state;

void init() {
    Arena* arena = ArenaAllocateMB(ARENA_MEMORY_MB);

    state = PushStruct(arena, State);

    if (!state->window.init("Game", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
	DEBUG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    state->inputManager.init();
    state->camera = createCamera();

    Editor::activate_editor(&state->editor);
    
    state->isRunning = true;
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

        #ifdef DEBUG_ENABLED
		DEBUG_LOG("this is debugger");
        #endif
    
    while (state->isRunning) {
        // float dt = calculateDeltaTime();
        // process_input(dt);
	process_input();
        update_game();
    }
    
    deinit();

    return 0;
}
