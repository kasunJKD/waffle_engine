#include "window.h"
#include "renderManager.h"
#include "entityManager.h"
#include "soundSystem.h"
#include "input.h"

#include <iostream>

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

void setupSoundEntity(SoundManager& soundManager, EntityManager& entityManager, SoundSystem& soundSystem) {
    Sound* sound = soundManager.GetSound("D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav");
    if (!sound) {
        std::cerr << "Sound not found!" << std::endl;
        return;
    }

    entity_id id = entityManager.CreateEntity();
    AudioEmitterComponent comp;
    comp.soundName = "D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav";
    comp.isPlaying = false;
    comp.loop = true;
    comp.volume = 1.0f;
    comp.pitch = 1.0f;

    soundSystem.GenAndBindOpenALOptions(sound, comp);

    entityManager.AddAudioEmitterComponent(id, comp);
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

uint32_t lastTicks = SDL_GetTicks();

float calculateDeltaTime() {
    Uint32 currentTicks = SDL_GetTicks();
    float deltaTime = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    return deltaTime;
}

int main(int argc, char* argv[]) {
    Window window;
    EntityManager entityManager;
    RenderManager renderManager;
    InputManager inputManager;

    // Sounds
    SoundManager soundManager;
    SoundSystem soundSystem;
    soundSystem.initialise(&soundManager, &entityManager);

    // Load sound
    soundManager.AddSound("D:\\Personal\\waffle_engine\\bin\\Debug\\Sandbox\\sounds\\sample-3s.wav");

    // Initialize the window
    if (!window.init("Sandbox", 800, 600)) {
        std::cerr << "Failed to initialize window." << std::endl;
        return -1;
    }

    inputManager.init();
    renderManager.init();

    initquad(entityManager);

    soundSystem.initializeListener();
    setupSoundEntity(soundManager, entityManager, soundSystem);

    // Main loop
    bool isRunning = true;

    while (isRunning) {
        uint32_t frameStart = SDL_GetTicks();

        float dt = calculateDeltaTime();

        inputManager.update(isRunning);
        if (!isRunning) break;
        updateMovableEntities(entityManager, inputManager, dt);

        soundSystem.update(dt);

        // // Poll for events
        // while (SDL_PollEvent(&event)) {
        //     if (event.type == SDL_QUIT) {
        //         isRunning = false; // Exit if the window is closed
        //     } else if (event.type == SDL_WINDOWEVENT) {
        //         if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
        //             int newWidth = event.window.data1;
        //             int newHeight = event.window.data2;
        //             window.resize(newWidth, newHeight); // Update viewport on resize
        //         }
        //     }
        // }

        // Clear the screen
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderManager.update(entityManager);

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
    window.cleanUp();

    SDL_Quit(); // Close SDL

    return 0;
}
