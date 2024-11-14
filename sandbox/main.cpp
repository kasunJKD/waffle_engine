#include "window.h"
#include "renderManager.h"
#include "entityManager.h"

#include <iostream>

entity_id initquad(EntityManager& entityManager);

//@TODO refactor to a time.h
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

    // Initialize the window with a title, width, and height
    if (!window.init("Sandbox", 800, 600)) {
        std::cerr << "Failed to initialize window." << std::endl;
        return -1;
    }

    renderManager.init();
    initquad(entityManager);

    // Main loop
    bool isRunning = true;
    SDL_Event event;

    //@TODO time.h
    uint32_t lastTicks = SDL_GetTicks();

    while (isRunning) {
        // Poll for events
        while (SDL_PollEvent(&event)) {
            float dt = calculateDeltaTime();

            if (event.type == SDL_QUIT) {
                isRunning = false; // Exit if the window is closed
            } else if (event.type == SDL_WINDOWEVENT) {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = event.window.data1;
                    int newHeight = event.window.data2;
                    window.resize(newWidth, newHeight); // Update viewport on resize
                }
            }
        }

        // Clear the screen
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        renderManager.update(entityManager);

        // Swap buffers to display the current frame
        window.swapBuffers();
    }

    // Cleanup resources before exiting
    renderManager.cleanUp();
    window.cleanUp();

    SDL_Quit(); // Close SDL

    return 0;
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
        {0.0f, 0.0f},      // position (x, y)
        0.0f,              // rotation (in degrees)
        {1.0f, 1.0f}       // scale (x, y)
    };
    entityManager.AddTransformComponent(id, transform);

    return id;
}