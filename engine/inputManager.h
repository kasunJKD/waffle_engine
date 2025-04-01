#pragma once

#include "window.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <unordered_map>
#include <glm/glm.hpp>

enum KeyState {
    NoState,
    Pressed,
    Held,
    Released,
    Scroll,
    ScrollFlip
};

struct InputManager {
    void init();
    void update(bool& isRunning, Window *window);

    glm::vec3 getMovementDirection() const;

    std::unordered_map<SDL_Keycode, glm::vec3> keyMappings;
    glm::vec3 movementDirection;

    void loadDefaultKeyMappings();

    // Key-related functions
    bool isKeyPressed(SDL_Keycode key) const;
    bool isKeyHeld(SDL_Keycode key) const;
    bool isKeyReleased(SDL_Keycode key) const;

    // Mouse-related functions
    bool isMouseButtonPressed(Uint8 button) const;
    bool isMouseButtonHeld(Uint8 button) const;
    bool isMouseButtonReleased(Uint8 button) const;
    bool isMouseButtonScrollFlip(Uint8 button) const;
    bool isMouseWheelScroll(Uint8 button) const;

    int getMouseX() const;
    int getMouseY() const;
    int getMouseWheelY() const;

    std::unordered_map<SDL_Keycode, KeyState> keyStates;
    std::unordered_map<Uint8, KeyState> mouseButtonStates;
    int mouseX = 0, mouseY = 0;
    int mouseWheelY = 0;

    void handleKeyEvent(const SDL_Event& event);
    void handleMouseEvent(const SDL_Event& event);
    void resetStates();
};
