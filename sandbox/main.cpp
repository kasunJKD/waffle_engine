#include "engine.h" // IWYU pragma: keep
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include <iostream>

#include <cstdio>
#include <cstdlib>

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288
#define WORLD_WIDTH 3072
#define WORLD_HEIGHT 1728

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; 

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

GLuint quadVAO, quadVBO;

void UpdateCamera(Camera &camera)
{
    // Orthographic 2D projection that is 512x288 in size
    // (left=0, right=512, top=0, bottom=288, near=-1, far=+1)
    camera.projection = glm::ortho(
        0.0f, camera.width,    // left, right
        camera.height, 0.0f,   // bottom, top  (note the flip if you like Y up vs. Y down)
        -1.0f, 1.0f
    );

    // The view is just a translation that offsets by the camera’s position in the world
    camera.view = glm::translate(glm::mat4(1.0f), 
                                 glm::vec3(-camera.position.x, -camera.position.y, 0.0f));
}


glm::mat4 projection_w = glm::ortho(0.0f, (float)WORLD_WIDTH, 0.0f, (float)WORLD_HEIGHT);

int main() {
    Window window;
    InputManager inputManager;


    // Initialize the window
    if (!window.init("Sandbox", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
        DEBUG_ERROR("Failed to initialize window.");
        return -1;
    }

    size_t arenaSize = 10 * (1024 * 1024); // For example, 1 MB.
    MEM::MemoryArena* arena = MEM::arena_create(arenaSize);
    if (!arena) {
        DEBUG_ERROR("Failed to create MemoryArena.");
        return 1;
    }

    ResourceManager* r_manager = createResourceManager(arenaSize);
    Resource* spritesheet = load(r_manager, "assets/tiled/testSpritesheet.png", nullptr, TEXTURE, "spritesheet");
    load(r_manager, "assets/test_game/testWorld1.png", nullptr, TEXTURE, "world1");
    load(r_manager, "assets/test_game/testWorld2.png", nullptr, TEXTURE, "world2");
    load(r_manager, "sandbox/shaders/quad.vert", "sandbox/shaders/quad.frag", SHADER, "quad");
    load(r_manager, "sandbox/fonts/Roboto.ttf", nullptr, FONT, "defaultfont", 48);
    load(r_manager, "sandbox/shaders/text.vert", "sandbox/shaders/text.frag", SHADER, "text");
    if (!spritesheet)
    {
        DEBUG_ERROR("spritesheet error");
        return -1;
    }

    //######test world rendering##########
    //#############################
    RenderSystem rendersys;
    initRenderSystem(&rendersys, r_manager);

    entity_system_init(arena);

    // Initialize the camera
    Entity* cam = create_entity("camera");
    cam->type = CAMERA;
    Camera camera;
    camera.id = cam->id;
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.width = 512.0f;
    camera.height = 288.0f;
    UpdateCamera(camera);
    
    Entity* we1 = create_entity("world1", WORLD, &rendersys);
    we1->type = WORLD;
    we1->shader_name = "quad";
    we1->texture_name = "world1";
    we1->VAO = quadVAO;
    Entity* we2 = create_entity("world2");
    we2->type = WORLD;
    we2->shader_name = "quad";
    we2->texture_name = "world2";
    we2->VAO = quadVAO;

    Entity* text_1_e = create_entity("text1");
    text_1_e->type = TEXT;
    text_1_e->color = glm::vec3(1.0f, 0.0f,0.0f);
    text_1_e->scale = 1.0;
    text_1_e->position = glm::vec3(0.0f, 20.0f, 0.0f);

    InitWorldQuad(we1);
    InitWorldQuad(we2);

    inputManager.init();

    bool isRunning = true;

    bool switch_world = false;
    while (isRunning) {
        #ifdef DEBUG_ENABLED
            reloadChangedShaders(r_manager);
        #endif

        uint32_t frameStart = SDL_GetTicks();
        //float dt = calculateDeltaTime();
        inputManager.update(isRunning);

        //###test rendering###########

        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Entity *e = (switch_world) ? we1: we2;

        RenderWorldTexture(r_manager->getResourceByName(e->texture_name)->data.i,
                           e,
                           &camera,
                           r_manager->getResourceByName(e->shader_name)->data.i);

        if (inputManager.isKeyPressed(SDLK_p)) {
            switch_world = !switch_world;

            std::cout << "P pressed! pauseActive is now " 
                << (switch_world ? "true" : "false") << std::endl;
        }

        {
            //text rendering
            RenderText_f1(r_manager->getResourceByName("text")->data.i, "Text is rendering", text_1_e, &camera);
        }

        window.swapBuffers();

        // Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    
    FontManager::DestroyInstance();
    MEM::arena_destroy(arena);
    destroyResourceManager(r_manager);
    window.cleanUp();
    MEM::arena_print_stats(arena);

    return 0;
}
