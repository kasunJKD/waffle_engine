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
constexpr uint64_t ARENA_MEMORY_MB_SCRATCH = 5;

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

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

State state_instance = {};
State* state = &state_instance;

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
    Arena* frame_buffer = ArenaAllocateMB(ARENA_MEMORY_MB_SCRATCH);

    state->arena = arena;
    state->asset_arena = asset_arena;
    state->scratch_buffer = frame_buffer;

    Window* win = PushStructZero(state->arena, Window);
    if (!win->init("Game", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
	DEBUG_ERROR("SDL_Init failed: %s", SDL_GetError());
        return;
    }

    state->window = win;

    //will be changed per games
    size_t number_of_shaders = 5;
    size_t number_of_textures = 5;
    size_t number_of_meshes = 5;


    AssetManager* asset_manager = PushStruct(asset_arena, AssetManager);
    *asset_manager = AssetManager_Create(asset_arena, number_of_meshes + number_of_shaders + number_of_textures);
    add_shader(asset_arena, asset_manager, "D:\\Personal\\waffle_engine\\shaders\\test.vert", "D:\\Personal\\waffle_engine\\shaders\\test.frag", "test_shader", TEXT_SHADER);
    add_texture(asset_manager, "D:\\Personal\\waffle_engine\\assets\\container.jpg", "image_container", TEST_TEXTURE);
    state->asset_manager = asset_manager;

    InputManager* inputManager = PushStructZero(state->arena, InputManager);
    state->inputManager = inputManager;
    state->inputManager->init();

    state->camera = createCamera(state->arena);
    state->entity_manager = create_entity_manager(100, arena);

    GLRENDER* glrenderer = PushStruct(state->arena, GLRENDER);
    state->gl_renderer = glrenderer;
    gl_render_init(state->gl_renderer);

    RenderSystem* renderSystem = PushStruct(state->arena, RenderSystem);
    state->render_system = renderSystem;
    renderSystem->frame_buffer = frame_buffer;
    renderSystem->command_capacity = 2048;
    renderSystem->drawCommands = PushArray(renderSystem->frame_buffer, RenderCommand, renderSystem->command_capacity);
    renderSystem->command_count = 0;

    create_render_system(state->render_system,  state->entity_manager, state->asset_manager, state->gl_renderer);

    Editor* editor = PushStruct(state->arena, Editor);
    state->editor = editor;
    Editor::activate_editor(state->editor);
    
    state->isRunning = true;

    test_create_entities(state->entity_manager);
}


void update_entities(State *state) {
    for(size_t i=0;state->render_system->entityManager->count > i; i++) {
        Entity entity = state->render_system->entityManager->entities[i];
        if(entity.flag & F_RENDER && entity.type == T_TEST) {
            Asset* test_asset = AssetManager_GetByName(state->asset_manager, "test_shader");
            Asset* test_texture = AssetManager_GetByName(state->asset_manager, "image_container");
            push_texture_command(state->render_system, entity.id, vec3(0.0,0.0,0.0), test_asset->shader.program, test_texture->texture.texture, state->gl_renderer->VAO);
        }

    }
}
// void build_draw_commands(RenderSystem *renderer, World *world, Arena *scratch)
// {
//     // Step 1. Collect visible entities (done earlier or inline)
//     VisibleEntity *visible_entities = world->visible_entities;
//     size_t visible_count = world->visible_count;
//
//     // Step 2. Allocate temporary arrays in scratch for sorting
//     temporary_memory temp = BeginTemporaryMemory(scratch);
//     SortEntry *sort_keys = PushArray(scratch, SortEntry, visible_count);
//
//     // Step 3. Generate sort keys (e.g., by texture, depth, layer)
//     for (size_t i = 0; i < visible_count; ++i) {
//         sort_keys[i].texture_id = visible_entities[i].texture->id;
//         sort_keys[i].depth = visible_entities[i].transform.position.z;
//         sort_keys[i].index = i;
//     }
//
//     // Step 4. Sort using the scratch arrays
//     quick_sort(sort_keys, visible_count, compare_sort_entry);
//
//     // Step 5. Emit RenderCommands into the transient arena
//     for (size_t i = 0; i < visible_count; ++i) {
//         VisibleEntity *v = &visible_entities[sort_keys[i].index];
//         push_mesh_command(renderer, v->mesh, v->texture, v->transform);
//     }
//
//     // Step 6. Release scratch memory
//     EndTemporaryMemory(temp);
// }

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
            if(state->editor->active) {
                state->editor->update();
            }
        #endif


        update_entities(state);

        //buid_draw_commeads() will process rendererqueue
        
        render_frame(state->render_system);

        state->window->swapBuffers();
        
}

void process_input() {
        state->inputManager->update(state->isRunning, state->window);
}

void deinit(){
    state->window->cleanUp();
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

// 🧱 Step-by-Step Breakdown
// 1️⃣ process_input()
//
// Purpose: Capture player intent, store key states, update movement inputs.
//
// Item	Arena	Reason
// InputManager struct	Permanent	Input system exists for entire game session
// Input states (keys, mouse, gamepad)	Permanent	Persist frame to frame
// Temp event queue	Transient (optional)	Reset each frame after processing
//
// ✅ Use: Permanent (for manager) + optional transient (for per-frame event list)
//
// 2️⃣ update_physics_and_collisions()
//
// Purpose: Move bodies, detect and resolve collisions.
//
// Item	Arena	Reason
// Entities & physics bodies	World	Persist with current level
// Collision pairs / contact data	Transient	Recomputed every frame
// Debug draw rectangles / lines	Transient	Draw once, cleared each frame
//
// ✅ Use: World + Transient
// → Entities live in world memory; collision work lists in transient memory.
//
// 3️⃣ update_entities()
//
// Purpose: Run game logic, AI, triggers, animations, etc.
//
// Item	Arena	Reason
// Entities	World	Same objects persist across frames
// Temporary script data / targets / lists	Transient	Created + destroyed per frame
// Created projectiles / spawned entities	World	Should persist until deleted manually
//
// ✅ Use: World + Transient
// → Logic manipulates world data, uses transient for temporary lookups or lists.
//
// 4️⃣ build_draw_commands()
//
// Purpose: Convert entity/world state into a renderable command list.
//
// Item	Arena	Reason
// Visible entity list	Transient	Built each frame
// RenderCommand buffer	Transient	Reset each frame
// Sorting / batching arrays	Scratch	Temporary during sorting only
//
// ✅ Use: Transient (for commands) + Scratch (inside sorting)
// → These get reset each frame after rendering.
//
// 5️⃣ render_frame()
//
// Purpose: Execute draw commands → FBO → screen.
//
// Item	Arena	Reason
// RenderSystem struct	Permanent	Persistent renderer system
// Framebuffer / shaders / pipelines	Permanent	Created once at init
// Draw commands	Transient	Built this frame only
// Post-process temp buffers	Transient	Reused each frame
//
// ✅ Use: Permanent (for systems) + Transient (for per-frame data)
//
// 🧠 Summary Table
// Step	Primary Arena	Secondary Arena(s)	Why
// process_input()	Permanent	Transient (for events)	Input system persists
// update_physics_and_collisions()	World	Transient	Entities in world; collision pairs temporary
// update_entities()	World	Transient	Game logic affects persistent entities
// build_draw_commands()	Transient	Scratch	Commands + sorting are frame-based
// render_frame()	Permanent	Transient	Renderer is global; command list temporary
