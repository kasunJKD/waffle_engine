#include "window.h"
#include "resourceManager.h"
#include "renderManager.h"
#include "entityManager.h"
#include "soundSystem.h"
#include "input.h"
#include "sceneManager.h"

#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // For glm::translate
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "defines.h"

#define SCREENSIZE_WIDTH 960
#define SCREENSIZE_HEIGTH 540
#define GAME_WIDTH 512
#define GAME_HEIGTH 288

const uint32_t TARGET_FPS = 60;
const uint32_t FRAME_DELAY = 1000 / TARGET_FPS; // Time per frame in milliseconds

//@todo move this to another .h
void updateMovableEntities(EntityManager& entityManager, InputManager& inputManager, float deltaTime) {
    glm::vec3 movementDirection = inputManager.getMovementDirection();

    for (auto it = entityManager.movables.begin(); it != entityManager.movables.end(); ++it) {
        entity_id entityID = it->first; // Access the entity ID
        MovableComponent& movable = it->second; // Access the MovableComponent

        if (entityManager.transforms.find(entityID) != entityManager.transforms.end()) {
            auto& transform = entityManager.transforms[entityID];
            transform.position += movementDirection * movable.speed * deltaTime;
        }
    }
}

entity_id setupSoundEntity(SoundManager& soundManager, EntityManager& entityManager, SoundSystem& soundSystem) {
    Sound* sound = soundManager.GetSound("D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav");
    if (!sound) {
        std::cerr << "Sound not found!" << std::endl;
        return -1;
    }

    entity_id id = entityManager.CreateEntity();
    AudioEmitterComponent comp;
    comp.soundName = "D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav";
    comp.isPlaying = true;
    comp.loop = true;
    comp.volume = 1.0f;
    comp.pitch = 1.0f;

    soundSystem.GenAndBindOpenALOptions(sound, comp);

    entityManager.AddAudioEmitterComponent(id, comp);

    return id;
}

entity_id initquad(EntityManager& entityManager)
{
    entity_id id = entityManager.CreateEntity();

    // Add a ShapeRectComponent with specific values
    Shape_RectComponent shapeRect = {
        {100.0f, 50.0f},   // size (width, height)
        {1.0f, 0.0f, 0.0f, 1.0f}, // color (RGBA) - red
        true               // filled
    };
    entityManager.AddShapeRectComponent(id, shapeRect);

    // Add a TransformComponent with specific values
    TransformComponent transform = {
        {0.0f, 0.0f, 0.0f},      // position (x, y)
        0.0f,              // rotation (in degrees)
        {1.0f, 1.0f}       // scale (x, y)
    };
    entityManager.AddTransformComponent(id, transform);
    entityManager.AddMovableComponent(id, { 100.0f, glm::vec3(0.0f) });

    return id;
}

entity_id initTiledMap_game(EntityManager& entityManager, RenderManager& renderManager)
{
    entity_id id = entityManager.CreateEntity();

    TiledMap mapData = renderManager.parseTiledJSON("assets/tiled/testmapAlldata.json");
    TiledMapComponent comp = {
        mapData
    };
    entityManager.AddTiledMapComponent(id, comp);
    return id;
}

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

int main(int argc, char* argv[]) {
    Window window;

    ResourceManager resourceManager;
    
    EntityManager entityManager;
    
    SceneManager sceneManager(&entityManager);
    sceneManager.createScene("Default");
    sceneManager.loadScene("Default");
    sceneManager.createScene("MainUI");
    //sceneManager.loadScene("MainUI");

    RenderManager renderManager; 
    InputManager inputManager;

    // Sounds
    SoundManager soundManager;
    SoundSystem soundSystem;
    soundSystem.initialise(&soundManager, &entityManager);

    // Load sound
    soundManager.AddSound("D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav");

    // Initialize the window
    if (!window.init("Sandbox", SCREENSIZE_WIDTH, SCREENSIZE_HEIGTH)) {
        std::cerr << "Failed to initialize window." << std::endl;
        return -1;
    }

    inputManager.init();
    renderManager.init();

    entity_id id = initquad(entityManager);
    sceneManager.addEntityToScene(id);

    soundSystem.initializeListener();
    entity_id soundid = setupSoundEntity(soundManager, entityManager, soundSystem);
    sceneManager.addEntityToScene(soundid);

    resourceManager.addResource("assets/tiled/testSpritesheet.png", "spritesheet", TEXTURE);
     
    // Initialize VAO and Shader Program
    renderManager.renderTileInit();
    glm::mat4 projection = glm::ortho(
        0.0f, (float)GAME_WIDTH,    // left, right
        (float)GAME_HEIGTH, 0.0f,   // bottom, top
        -1.0f, 1.0f                  // near, far
    ); 
    // 5) Set that projection once (or each frame)
    glUseProgram(renderManager.spShader);
    GLint projLoc = glGetUniformLocation(renderManager.spShader, "projection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    GLuint textureID2 = resourceManager.getResource<GLuint>("spritesheet", ResourceType::TEXTURE);

    auto sprites = renderManager.initializeSprites();

    TiledMap mapData = renderManager.parseTiledJSON("assets/tiled/testmapAlldata.json");
    //@TODO add tiled map to component and load and render by component
    //@TODO addEntityToScene(entity, sceneName) modify this
    //entity_id mapID = initTiledMap_game(entityManager, renderManager);
    //sceneManager.addEntityToScene(mapID);

    for (const auto& entry : sprites) {
    SpriteID id = entry.first;            // Access the key
    const Sprite& sprite = entry.second; // Access the value

    std::cout << "SpriteID: " << static_cast<int>(id)
              << ", Atlas Offset: (" << sprite.atlasOffset.x << "," << sprite.atlasOffset.y << ")"
              << ", Size: (" << sprite.size.x << "," << sprite.size.y << ")"
              << ", GID: " << sprite.gid << "\n";
}

for (const auto& group : mapData.objectGroups) {
        std::cout << "ObjectGroup name: " << group.name << "\n";
        if (group.name == "playerstart") {
            // We expect maybe 1 object in playerstart?
            for (const auto& obj : group.objects) {
                std::cout << "  Found player start object. "
                          << "x=" << obj.x << ", y=" << obj.y << "\n";
            }
        }
    }
    // Main loop
    bool isRunning = true;

    while (isRunning) {
        uint32_t frameStart = SDL_GetTicks();
        float dt = calculateDeltaTime();

        inputManager.update(isRunning);
        if (!isRunning) break;
        updateMovableEntities(entityManager, inputManager, dt);

        soundSystem.update(dt, sceneManager);

        // Clear the screen
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //@todo refactor into update and scene manager
renderManager.renderLayers(mapData.layers,
            16,
            9,
            sprites,
            textureID2,
            32,
            /* atlasWidth  */ (float)GAME_WIDTH,
            /* atlasHeight */ (float)GAME_HEIGTH
        );
        renderManager.update(entityManager, sceneManager);

        Sprite& playerSpriteViaSubscript = sprites[PLAYER];
        renderManager.renderTile_P(playerSpriteViaSubscript, textureID2, (float)GAME_WIDTH, (float)GAME_HEIGTH);

        // Swap buffers to display the current frame
        window.swapBuffers();

        // Frame limiting
        uint32_t frameTime = SDL_GetTicks() - frameStart;
        if (frameTime < FRAME_DELAY) {
            SDL_Delay(FRAME_DELAY - frameTime);
        }
    }

    // Cleanup resources before exiting
    renderManager.cleanUp();
    soundSystem.clean();
    soundManager.RemoveSounds();
    resourceManager.clearResources();
    window.cleanUp();

    SDL_Quit(); // Close SDL

    return 0;
}
