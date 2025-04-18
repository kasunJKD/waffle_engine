#include "allocator.h"
#include "debug.h"
#include "editor.h"
#include "engine.h" // IWYU pragma: keep
#include "entity.h"
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "save.h"

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288
#define WORLD_WIDTH 3072
#define WORLD_HEIGHT 1728

#define ENTITY_ARENA_SIZE    2* (1024 * 1024)

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; 

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

void UpdateCamera(Camera &camera)
{
    camera.projection = glm::ortho(
        0.0f, camera.width,    // left, right
        camera.height, 0.0f,   // bottom, top  (note the flip if you like Y up vs. Y down)
        -10.0f, 10.0f
    );

    camera.view = glm::translate(glm::mat4(1.0f), 
                                 glm::vec3(-camera.position.x, -camera.position.y, camera.position.z));
}

struct State {
    Window window;
    InputManager inputManager;
    ResourceManager resourceManager;
    SpriteManager spriteManager;
    RenderSystem renderSystem;
    EntitySystem entitiySystem;
    
    GLuint quadVAO, quadVBO;
    Pool_Allocator::Pool persistant_storage; //for resources
    Temp_Allocator::TempArena entity_storage; //for entities

    #ifdef DEBUG_ENABLED
        Editor editor;
    #endif

    Camera* camptr;
    Camera camera;

    bool isRunning;

    Entity* player;
    Entity* text_1;
};

static unsigned char temp_arena_memory[ENTITY_ARENA_SIZE];
State state = {};

void init() {
    if (!state.window.init("Game", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
        DEBUG_ERROR("Failed to initialize window.");
        return;
    }
    
    Pool_Allocator::pool_init(&state.persistant_storage, temp_arena_memory, ENTITY_ARENA_SIZE, 64);

    Temp_Allocator::temp_arena_init(&state.entity_storage, temp_arena_memory, ENTITY_ARENA_SIZE);

    state.resourceManager = *createResourceManager(&state.persistant_storage);
    
    load_game_data("D:/Personal/waffle_2/waffle_engine/sandbox/save.adf", &state.resourceManager);

    Resource* spritesheet = load(&state.resourceManager, "assets/tiled/testSpritesheet.png", nullptr, TEXTURE, "spritesheet");
    load(&state.resourceManager, "assets/test_game/testWorld1.png", nullptr, TEXTURE, "world1");
    load(&state.resourceManager, "assets/test_game/testWorld2.png", nullptr, TEXTURE, "world2");
    load(&state.resourceManager, "sandbox/shaders/quad.vert", "sandbox/shaders/quad.frag", SHADER, "quad");
    load(&state.resourceManager, "sandbox/fonts/Roboto.ttf", nullptr, FONT, "defaultfont", 48);
    load(&state.resourceManager, "sandbox/shaders/text.vert", "sandbox/shaders/text.frag", SHADER, "text");
    load(&state.resourceManager, "sandbox/shaders/sprite.vert", "sandbox/shaders/sprite.frag", SHADER, "sprite");
    if (!spritesheet)
    {
        DEBUG_ERROR("spritesheet error");
        return;
    }

    state.spriteManager = *createSpriteManager(&state.persistant_storage, &state.resourceManager);
    loadSpritesMapping(&state.spriteManager);

    initRenderSystem(&state.renderSystem, &state.resourceManager);

    entity_system_init(&state.entity_storage);
    state.entitiySystem = *get_entity_manager();

    state.inputManager.init();

    state.isRunning = true;

    #ifdef DEBUG_ENABLED
        state.editor.init_editor(&state.spriteManager);
    #endif

    // Initialize the camera
    Camera camera;
    camera.type = CAMERA;
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.width = 512.0f;
    camera.height = 288.0f;
    UpdateCamera(camera);
    state.camera = camera;

    state.camptr = &state.camera;
    
    Entity* text_1_e = create_entity("text1");
    text_1_e->type = TEXT;
    text_1_e->color = glm::vec3(1.0f, 0.0f,0.0f);
    text_1_e->scale = 1.0;
    text_1_e->position = glm::vec3(0.0f, 20.0f, 1.0f);
    state.text_1 = text_1_e;

    Entity* player = create_entity("player");
    player->type = PLAYER;
    player->scale = 1.0;
    player->position = glm::vec3(0.0f, 10.0f, 3.0f);
    player->sprite = state.spriteManager.getSprite("player");
    player->active = true;
    player->shader_name = "sprite";
    player->VAO = state.quadVAO;
    state.player = player;

}

void process_input(float dt) {
        state.inputManager.update(state.isRunning, &state.window);

        #ifdef DEBUG_ENABLED
            if (state.inputManager.isKeyPressed(SDLK_TAB)) {
                if (!state.editor.active) {
                    state.camptr = &state.editor.camera;
                    state.editor.activate_editor();
                } else {
                    DEBUG_LOG("editor deactivated");
                    state.camptr = &state.camera;
                    state.editor.active = false;
                }
            } 
            if(state.editor.active) {
                state.editor.update_editor(&state.inputManager, dt);
                state.camptr = &state.editor.camera;
            } 
            if (state.inputManager.isKeyPressed(SDLK_q)) {
                    DEBUG_LOG("game file save fired");
                    save_game_data("D:/Personal/waffle_2/waffle_engine/sandbox/save.adf", get_entity_manager());
                } 
        #endif
}

void update_game() {
        #ifdef DEBUG_ENABLED
            //@hotreload shaders
            reloadChangedShaders(&state.resourceManager);
        #endif
        
        uint32_t frameStart = SDL_GetTicks();

        glClearColor(1.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            //text rendering
            RenderText_f1(state.resourceManager.getResourceByName("text")->data.i, "this is text", state.text_1, state.camptr);
        }

        {
            renderSprite(state.resourceManager.getResourceByName("sprite")->data.i, state.player, state.camptr, &state.renderSystem);
        }

        #ifdef DEBUG_ENABLED
            if(state.editor.active) {
                state.editor.draw_editor();
            }
        #endif

        state.window.swapBuffers();

        // Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
}

void deinit(){
    #ifdef DEBUG_ENABLED
        state.editor.deinit_editor();
    #endif
    FontManager::DestroyInstance();
    destroyResourceManager(&state.resourceManager);
    state.window.cleanUp();

    Temp_Allocator::temp_arena_free_all(&state.entity_storage);
}

int main() {
    init();
    
    while (state.isRunning) {
        float dt = calculateDeltaTime();
        process_input(dt);
        update_game();
    }
    
    deinit();

    return 0;
}
