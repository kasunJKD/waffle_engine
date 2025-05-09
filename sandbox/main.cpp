#include "allocator.h"
#include "debug.h"
#include "defines.h"
#include "editor.h"
#include "engine.h" // IWYU pragma: keep
#include "entity.h"
#include "glad/glad.h"
#include <cstdint>
#include <cstdio>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/vector_float3.hpp"
#include "glm/fwd.hpp"
#include "glrenderSystem.h"
#include "save.h"

#include "stb_image.h"

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
struct State;
// --------------------------------------------------
// A minimal Scene type
// --------------------------------------------------
struct Scene {
    std::string name;
    std::vector<entityId> entities;
    bool           active = false;
  
    // build all the entities for this scene (called once)
    std::function<void(State&)>  init;
    // clean up if you want to destroy entities when you leave
    std::function<void(State&)>  teardown;
};

// --------------------------------------------------
// SceneManager lives in your State
// --------------------------------------------------
struct SceneManager {
    std::unordered_map<std::string, Scene> scenes;
    Scene* current = nullptr;

    void addScene(Scene&& s) {
        scenes.emplace(s.name, std::move(s));
    }

    void switchTo(State& S, const std::string& name) {
        // teardown current
        if (current && current->active) {
            if (current->teardown) current->teardown(S);
            current->active = false;
        }
        // init new
        auto it = scenes.find(name);
        assert(it != scenes.end());
        current = &it->second;
        if (!current->active) {
            if (current->init) current->init(S);
            current->active = true;
        }
    }
};

struct State {
    Window window;
    InputManager inputManager;
    ResourceManager resourceManager;
    SpriteManager spriteManager;
    RenderSystem renderSystem;
    EntitySystem* entitiySystem;
    SceneManager scenemgr;
    
    GLuint quadVAO, quadVBO;
    Pool_Allocator::Pool persistant_storage; //for resources
    Temp_Allocator::TempArena entity_storage; //for entities

    #ifdef DEBUG_ENABLED
        Editor editor;
        GLuint debugVAO, debugVBO;
        GLint  lineShader, uMVPLoc, uColorLoc;
    #endif

    Camera* camptr;
    Camera camera;
    Entity* player;
    Entity* text_1;

    bool isRunning;
};

void loadFromBluePrint(State &state, const char* filename, int tileW, int tileH, const std::unordered_map<uint32_t, const char*>& colorToSpriteNameMap, std::string sceneName) {
    int w, h, channels;
    unsigned char* data = stbi_load(filename, &w, &h, &channels, 4);
    DEBUG_ASSERT(data, "loadFromBlueprint stbi load error");

    int row = h / tileH;
    int cols = w / tileW;

    for (int y = 0; y < row; y++) {
        for (int x = 0; x < cols; x++) {
            int px = (x * tileW) + (tileW / 2);
            int py = (y * tileH) + (tileH / 2);

            unsigned char* pixelOffset = data + ((py * w) + px) * 4;
            unsigned char r = pixelOffset[0];
            unsigned char g = pixelOffset[1];
            unsigned char b = pixelOffset[2];
            unsigned char a = pixelOffset[3];

            uint32_t hex =
                (uint32_t(r) << 24) |
                (uint32_t(g) << 16) |
                (uint32_t(b) <<  8) |
                (uint32_t(a) <<  0);

            auto it = colorToSpriteNameMap.find(hex);
            if (it != colorToSpriteNameMap.end()) {
                printf(
                  "  [blueprint] Hit color 0x%08X at tile (%d,%d) → sprite '%s'\n",
                  hex, x, y, it->second
                );
            }
            if (it == colorToSpriteNameMap.end()) continue;

            const char* spriteName = it->second;

    //player->sprite = state.spriteManager.getSprite("player");
            Sprite* sprite = state.spriteManager.getSprite(spriteName);
            Entity* e = create_entity(spriteName);
            e->sprite = sprite;
            e->position = glm::vec3(x*tileW, y*tileH, 0.0f);
            e->scale    = 1.0f;

            // AABB half‐extents:
            e->halfSize = { 16 , 16};
             e->colliderOffset = { 0.0f,  0.0f };
            // Mark it collidable if you want physics on it:
            e->collidable = false;
            if (std::strcmp(spriteName, "water") == 0) {
             e->collidable = true;
            
            }
            e->active = true;

            state.scenemgr.scenes[sceneName].entities.push_back(e->id);

        }
    }
    stbi_image_free(data);
}

glm::vec2 getColliderCenter(const Entity& e) {
    // sprite origin is top‐left. colliderOffset is how many pixels
    // down/right from that to the TOP‐LEFT corner of the smaller box.
    glm::vec2 boxTopLeft = {
        e.position.x + e.colliderOffset.x,
        e.position.y + e.colliderOffset.y
    };
    // center = topLeft + halfSize
    return {
      boxTopLeft.x + e.halfSize.x,
      boxTopLeft.y + e.halfSize.y
    };
}

bool AABBOverlap(const Entity& a, const Entity& b) {
    glm::vec2 ca = getColliderCenter(a);
    glm::vec2 cb = getColliderCenter(b);
    float dx = std::abs(ca.x - cb.x);
    float dy = std::abs(ca.y - cb.y);
    return dx <= (a.halfSize.x + b.halfSize.x)
        && dy <= (a.halfSize.y + b.halfSize.y);
}
void moveWithCollision(Entity& e,
                       const glm::vec2& delta,
                       const std::vector<Entity*>& colliders)
{
    glm::vec3 oldPos = e.position;
    e.position.x += delta.x;
    e.position.y += delta.y;

    for (auto* other : colliders) {
        if (other == &e || !other->collidable) continue;
        if (AABBOverlap(e, *other)) {
            e.position = oldPos;
            break;
        }
    }
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
         "sandbox/shaders/line.vert",
         "sandbox/shaders/line.frag",
         SHADER, "line");
    if (!spritesheet)
    {
        DEBUG_ERROR("spritesheet error");
        return;
    }

    state.spriteManager = *createSpriteManager(&state.persistant_storage, &state.resourceManager);
    loadSpritesMapping(&state.spriteManager);

    initRenderSystem(&state.renderSystem, &state.resourceManager);

    entity_system_init(&state.entity_storage);
    state.entitiySystem = get_entity_manager();

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
    player->position = glm::vec3(80.0f, 80.0f, 3.0f);
    player->sprite = state.spriteManager.getSprite("player");
    player->active = true;
    player->shader_name = "sprite";
    player->VAO = state.quadVAO;
float spriteW = 32.0f, spriteH = 64.0f;
// halfSize = {halfWidth, halfHeight} of the smaller box
player->halfSize       = { spriteW * 0.5f,  spriteH * 0.5f * 0.5f }; 
//           = {16, 16}

// colliderOffset.x = 0  (box is full width, so no horizontal shift)
// colliderOffset.y = spriteH - boxHeight = 64 - 32 = 32
player->colliderOffset = { 0.0f, spriteH - (player->halfSize.y * 2.5f) };
    player->collidable = true;
    state.player = player;


state.scenemgr.addScene(Scene{
    // [0] name
    "OverWorld",
    // [1] entities — default‐constructed to empty
    {},
    // [2] active    — default‐constructed to false
    false,
    // [3] init
    [](State &s) {
        static std::unordered_map<uint32_t, const char*> colorToSprite = {
            { 0x00FF16FF, "grass" },  
            { 0x0000FFFF, "water" },  
        };
        loadFromBluePrint(
            s,
            "assets/test_game/testWorld1.png",
            32, 32,
            colorToSprite,
            "OverWorld"
        );
    },
    // [4] teardown
    [](State &s) {
        // remove all spawned entities:
        auto &ents = s.scenemgr.scenes["OverWorld"].entities;
        for (auto id : ents) remove_entity(id);
        ents.clear();
    }
});

    state.scenemgr.switchTo(state, "OverWorld");
    printf("%s\n", state.scenemgr.current->name.c_str());
printf("=== Entity Dump (count = %zu) ===\n", state.entitiySystem->entity_count);
    for (size_t i = 0; i < state.entitiySystem->entity_count; ++i) {
        const Entity& e = state.entitiySystem->entities[i];
        printf(
            "Entity[%2zu] id=%3u, name=\"%s\", type=%d, world=%d, active=%s\n"
            "          pos=(%.1f, %.1f, %.1f), scale=%.1f\"\n",
            i,
            e.id,
            e.name ? e.name : "(null)",
            e.type,
            e.worldType,
            e.active ? "yes" : "no",
            e.position.x, e.position.y, e.position.z,
            e.scale
        );
    }
    printf("===============================\n");

    #ifdef DEBUG_ENABLED
        // get program handle
        state.lineShader = state.resourceManager
                       .getResourceByName("line")->data.i;

        // get uniform locations
        state.uMVPLoc   = glGetUniformLocation(state.lineShader, "uMVP");
        state.uColorLoc = glGetUniformLocation(state.lineShader, "uColor");

        // create a dynamic VBO for up to 4 verts
        glGenVertexArrays(1, &state.debugVAO);
        glGenBuffers(1, &state.debugVBO);
        glBindVertexArray(state.debugVAO);
        glBindBuffer(GL_ARRAY_BUFFER, state.debugVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float)*2*4, nullptr, GL_DYNAMIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), (void*)0);
        glBindVertexArray(0);
    #endif

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

void update_game(float dt) {
        #ifdef DEBUG_ENABLED
            //@hotreload shaders
            reloadChangedShaders(&state.resourceManager);
        #endif
        
        uint32_t frameStart = SDL_GetTicks();

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        static std::vector<Entity*> colliders;
        colliders.clear();

        for (auto id : state.scenemgr.current->entities) {
            Entity& ent = state.entitiySystem->entities[id];
            if (ent.active && ent.collidable)
                colliders.push_back(&ent);
        }
        // include player too if stored separately:
        if (state.player->active && state.player->collidable)
            colliders.push_back(state.player);

        // 2) Compute delta movement
        glm::vec3 dir3 = state.inputManager.getMovementDirection();
        glm::vec2 dir  = { dir3.x, dir3.y };
        if (glm::length(dir) > 0.0f) dir = glm::normalize(dir);
        glm::vec2 movement = dir * 100.0f * dt;  // e.g. 100px/sec * dt

        // 3) Move player with collision
        moveWithCollision(*state.player, movement, colliders);
   //only use follow cam in overworld
  {
    float px = state.player->position.x
             + state.player->halfSize.x
             + state.player->colliderOffset.x;
    float py = state.player->position.y
             + state.player->halfSize.y
             + state.player->colliderOffset.y;

    state.camera.position.x = px - (state.camera.width  * 0.5f);
    state.camera.position.y = py - (state.camera.height * 0.5f);

    UpdateCamera(state.camera);
  }

        {
            for (auto id : state.scenemgr.current->entities) {
                auto& ent = state.entitiySystem->entities[id];
                if (!ent.sprite) continue;          // sanity check
                renderSprite(
                    state.resourceManager.getResourceByName("sprite")->data.i,
                    &ent,
                    state.camptr,
                    &state.renderSystem
                );
            }
        }

        {
            //text rendering
            RenderText_f1(state.resourceManager.getResourceByName("text")->data.i, "this is text", state.text_1, state.camptr);
        }

        {
            renderSprite(state.resourceManager.getResourceByName("sprite")->data.i, state.player, state.camptr, &state.renderSystem);
        }

#ifdef DEBUG_ENABLED
    auto& cam = *state.camptr;
    glm::mat4 mvp = cam.projection * cam.view;

    glUseProgram(state.lineShader);
    glUniformMatrix4fv(state.uMVPLoc, 1, GL_FALSE, &mvp[0][0]);
    glUniform3f(state.uColorLoc, 1.0f, 0.0f, 0.0f); // red debug boxes
    glBindVertexArray(state.debugVAO);

    // Build collider list (clear old)
    std::vector<Entity*> debugColliders;
    for (auto id : state.scenemgr.current->entities) {
        Entity& e = state.entitiySystem->entities[id];
        if (e.active && e.collidable)
            debugColliders.push_back(&e);
    }
    if (state.player->active && state.player->collidable)
        debugColliders.push_back(state.player);

    // Draw each box
    for (auto* e : debugColliders) {
        // Top-left of collider in world coords
        float x = e->position.x + e->colliderOffset.x;
        float y = e->position.y + e->colliderOffset.y;
        // Full size
        float w = e->halfSize.x * 2.0f;
        float h = e->halfSize.y * 2.0f;

        float verts[8] = {
            x,   y,
            x+w, y,
            x+w, y+h,
            x,   y+h
        };

        glBindBuffer(GL_ARRAY_BUFFER, state.debugVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_LINE_LOOP, 0, 4);
    }

    glBindVertexArray(0);
    glUseProgram(0);
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
        glDeleteBuffers(1, &state.debugVBO);
        glDeleteVertexArrays(1, &state.debugVAO);
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
        update_game(dt);
    }
    
    deinit();

    return 0;
}
