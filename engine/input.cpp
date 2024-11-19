#include "input.h"
#include <iostream>

void InputManager::init() {
    // Initialize any input-related state if needed
    keyStates.clear();
    mouseButtonStates.clear();
    mouseX = 0;
    mouseY = 0;

    movementDirection = glm::vec3(0.0f);
    loadDefaultKeyMappings();
}

void InputManager::update(bool& isRunning) {
    resetStates(); // Reset transitional states like Pressed and Released

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                handleKeyEvent(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                handleMouseEvent(event);
                break;
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            case SDL_QUIT:
                std::cout << "Quit event received." << std::endl;
                isRunning = false;
                break;
            default:
                break;
        }
    }

    movementDirection = glm::vec3(0.0f);

    const Uint8* state = SDL_GetKeyboardState(NULL);
    for (auto it = keyMappings.begin(); it != keyMappings.end(); ++it) {
        SDL_Keycode key = it->first;
        glm::vec3 direction = it->second;
        if (state[SDL_GetScancodeFromKey(key)]) {
            movementDirection += direction;
        }
    }
}

bool InputManager::isKeyPressed(SDL_Keycode key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::isKeyHeld(SDL_Keycode key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::Held;
}

bool InputManager::isKeyReleased(SDL_Keycode key) const {
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second == KeyState::Released;
}

bool InputManager::isMouseButtonPressed(Uint8 button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second == KeyState::Pressed;
}

bool InputManager::isMouseButtonHeld(Uint8 button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second == KeyState::Held;
}

bool InputManager::isMouseButtonReleased(Uint8 button) const {
    auto it = mouseButtonStates.find(button);
    return it != mouseButtonStates.end() && it->second == KeyState::Released;
}

int InputManager::getMouseX() const {
    return mouseX;
}

int InputManager::getMouseY() const {
    return mouseY;
}

void InputManager::handleKeyEvent(const SDL_Event& event) {
    SDL_Keycode key = event.key.keysym.sym;
    if (event.type == SDL_KEYDOWN) {
        if (keyStates[key] == KeyState::NoState || keyStates[key] == KeyState::Released) {
            keyStates[key] = KeyState::Pressed;
        } else {
            keyStates[key] = KeyState::Held;
        }
    } else if (event.type == SDL_KEYUP) {
        keyStates[key] = KeyState::Released;
    }
}

void InputManager::handleMouseEvent(const SDL_Event& event) {
    Uint8 button = event.button.button;
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        if (mouseButtonStates[button] == KeyState::NoState || mouseButtonStates[button] == KeyState::Released) {
            mouseButtonStates[button] = KeyState::Pressed;
        } else {
            mouseButtonStates[button] = KeyState::Held;
        }
    } else if (event.type == SDL_MOUSEBUTTONUP) {
        mouseButtonStates[button] = KeyState::Released;
    }
}

void InputManager::resetStates() {
    for (auto& pair : keyStates) {
        KeyState& state = pair.second;
        if (state == KeyState::Pressed) {
            state = KeyState::Held;
        } else if (state == KeyState::Released) {
            state = KeyState::NoState;
        }
    }

    for (auto& pair : mouseButtonStates) {
        KeyState& state = pair.second;
        if (state == KeyState::Pressed) {
            state = KeyState::Held;
        } else if (state == KeyState::Released) {
            state = KeyState::NoState;
        }
    }
}

glm::vec3 InputManager::getMovementDirection() const {
    return movementDirection;
}

void InputManager::loadDefaultKeyMappings() {
    keyMappings[SDLK_w] = glm::vec3(0.0f, 1.0f, 0.0f);   // Move up
    keyMappings[SDLK_s] = glm::vec3(0.0f, -1.0f, 0.0f);  // Move down
    keyMappings[SDLK_a] = glm::vec3(-1.0f, 0.0f, 0.0f);  // Move left
    keyMappings[SDLK_d] = glm::vec3(1.0f, 0.0f, 0.0f);   // Move right
}