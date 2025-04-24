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
#include "glrenderSystem.h"
#include "save.h"

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288
#define WORLD_WIDTH 3072
#define WORLD_HEIGHT 1728
const glm::vec2 gameBounds  = glm::vec2(GAME_WIDTH, GAME_HEIGTH);
const glm::vec2 worldBounds = glm::vec2(WORLD_WIDTH, WORLD_HEIGHT);

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
        GLuint debugVAO = 0, debugVBO = 0;
        GLuint debugShader = 0;
    #endif

    Camera* camptr;
    Camera camera;

    bool isRunning;

    Entity* player;
    Entity* text_1;
    Entity* light;
    Entity* testquad;
};

void UpdateCamera(Camera &camera)
{
    float aspect   = camera.width / camera.height; // e.g. 512/288
    float halfH    = camera.height * 0.5f;        // 144 units
    float fovyDeg  = 60.0f;                       // pick a vertical FOV you like
    float fovy     = glm::radians(fovyDeg);

    float distance = halfH / tan(fovy * 0.5f);

    // 3) Set your camera position.z to that distance:
    camera.position.z = distance;

    // 4) Build the projection (and flip Y if that’s your convention):
    camera.projection = glm::perspective(fovy, aspect, 0.1f, distance + 1000.0f);
    camera.projection[1][1] *= -1.0f;

    // 5) Look from (x,y,z) toward the plane z=0:
    camera.view = glm::lookAt(
        glm::vec3(camera.position.x, camera.position.y, camera.position.z),
        glm::vec3(camera.position.x, camera.position.y, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}

void drawRect(State &S, const glm::vec2 &size, const glm::vec3 &color) {
    // upload shader + camera
    glUseProgram(S.debugShader);
    glUniformMatrix4fv(
      glGetUniformLocation(S.debugShader, "u_Proj"),
      1, GL_FALSE,
      glm::value_ptr(S.camptr->projection)
    );
    glUniformMatrix4fv(
      glGetUniformLocation(S.debugShader, "u_View"),
      1, GL_FALSE,
      glm::value_ptr(S.camptr->view)
    );
    glUniform3fv(
      glGetUniformLocation(S.debugShader, "u_Color"),
      1,
      glm::value_ptr(color)
    );

    // build a closed‐loop rectangle from (0,0) to (size.x,size.y)
    float verts[10] = {
      0.0f,        0.0f,
      size.x,      0.0f,
      size.x,      size.y,
      0.0f,        size.y,
      0.0f,        0.0f
    };

    glBindBuffer(GL_ARRAY_BUFFER, S.debugVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);

    glBindVertexArray(S.debugVAO);
      glDrawArrays(GL_LINE_STRIP, 0, 5);
    glBindVertexArray(0);

    glUseProgram(0);
}



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
    load(&state.resourceManager,
     "sandbox/shaders/debug.vert",
     "sandbox/shaders/debug.frag",
     SHADER,
     "debug");
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
        state.debugShader = state.resourceManager
                               .getResourceByName("debug")
                               ->data.i;

        // create a dynamic VBO for up to 5 points (a closed rectangle):
        glGenVertexArrays(1, &state.debugVAO);
        glGenBuffers(1, &state.debugVBO);

        glBindVertexArray(state.debugVAO);
          glBindBuffer(GL_ARRAY_BUFFER, state.debugVBO);
          glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*5, nullptr, GL_DYNAMIC_DRAW);
          // aPos = vec2
          glEnableVertexAttribArray(0);
          glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glBindVertexArray(0);
    #endif

    // Initialize the camera
    Camera camera;
    camera.type = CAMERA;
    camera.width  = float(GAME_WIDTH);
    camera.height = float(GAME_HEIGTH);
    camera.position.x = GAME_WIDTH  * 0.5f;
    camera.position.y = GAME_HEIGTH * 0.5f;
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

    Entity* light_ = create_entity("light");
    light_->type = LIGHT;
    light_->scale = 10.0;
    light_->position = glm::vec3(100.0f, 10.0f, 10.0f);
    light_->active = true;
    light_->color = glm::vec3(1.0f, 1.0f, 1.0f);
    light_->VAO = state.quadVAO;
    light_->VBO = state.quadVBO;
    light_->shader_name = "quad";
    state.light = light_;

    Entity* quad_ = create_entity("quad");
    quad_->type = QUAD;
    quad_->scale = 100.0;
    quad_->position = glm::vec3(100.0f, 10.0f, 1.0f);
    quad_->active = true;
    quad_->color = glm::vec3(1.0f, 0.5f, 0.31f);
    quad_->VAO = state.quadVAO;
    quad_->VBO = state.quadVBO;
    quad_->shader_name = "quad";
    state.testquad = quad_;

    initTestQuad(light_);
    initTestQuad(quad_);

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            //text rendering
            RenderText_f1(state.resourceManager.getResourceByName("text")->data.i, "this is text", state.text_1, state.camptr);
        }

        {
            renderSprite(state.resourceManager.getResourceByName("sprite")->data.i, state.player, state.camptr, &state.renderSystem);
        }
        
        {
            //test lights
            renderTestQuad(state.testquad, state.camptr, state.resourceManager.getResourceByName("quad")->data.i);
            renderLightQuad(state.light, state.camptr, state.resourceManager.getResourceByName("quad")->data.i);
        }

        #ifdef DEBUG_ENABLED
            if(state.editor.active) {
                state.editor.draw_editor();
            }
            // draw the world bounds in green:
            drawRect(state, worldBounds, glm::vec3(0.0f, 1.0f, 0.0f));

            // draw the game‐viewport bounds in red:
            drawRect(state, gameBounds,  glm::vec3(1.0f, 0.0f, 0.0f));
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
