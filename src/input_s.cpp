#include "input_s.h"
#include <iostream>

#ifdef DEBUG_ENABLED
#include "backends/imgui_impl_sdl2.h"
#endif
void InputManager::init() {
    // Initialize any input-related state if needed
    //keyStates.clear();
    //mouseButtonStates.clear();
    mouseX = 0;
    mouseY = 0;
    
    //int mouseWheelX = 0;
    mouseWheelY = 0;

    //movementDirection = glm::vec3(0.0f);
    //loadDefaultKeyMappings();
}

void InputManager::update(bool& isRunning, Window* window) {
    //resetStates(); // Reset transitional states like Pressed and Released

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        #ifdef DEBUG_ENABLED    
            ImGui_ImplSDL2_ProcessEvent(&event); // Forward your event to backend
        #endif

        switch (event.type) {
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                //handleKeyEvent(event);
                break;
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
                //handleMouseEvent(event);
                break;
            case SDL_MOUSEMOTION:
                mouseX = event.motion.x;
                mouseY = event.motion.y;
                break;
            case SDL_MOUSEWHEEL:
                //handleMouseEvent(event);
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED && window) {
                    window->resize(event.window.data1, event.window.data2);
                }
                break;
            case SDL_QUIT:
                std::cout << "Quit event received." << std::endl;
                isRunning = false;
                break;
            default:
                break;
       }
    }

    //movementDirection = glm::vec3(0.0f);

    // const Uint8* state = SDL_GetKeyboardState(NULL);
    // for (auto it = keyMappings.begin(); it != keyMappings.end(); ++it) {
    //     SDL_Keycode key = it->first;
    //     glm::vec3 direction = it->second;
    //     if (state[SDL_GetScancodeFromKey(key)]) {
    //         movementDirection += direction;
    //     }
    // }
}

// bool InputManager::isKeyPressed(SDL_Keycode key) const {
//     auto it = keyStates.find(key);
//     return it != keyStates.end() && it->second == KeyState::Pressed;
// }
//
// bool InputManager::isKeyHeld(SDL_Keycode key) const {
//     auto it = keyStates.find(key);
//     return it != keyStates.end() && it->second == KeyState::Held;
// }
//
// bool InputManager::isKeyReleased(SDL_Keycode key) const {
//     auto it = keyStates.find(key);
//     return it != keyStates.end() && it->second == KeyState::Released;
// }
//
// bool InputManager::isMouseButtonPressed(Uint8 button) const {
//     auto it = mouseButtonStates.find(button);
//     return it != mouseButtonStates.end() && it->second == KeyState::Pressed;
// }
//
// bool InputManager::isMouseButtonHeld(Uint8 button) const {
//     auto it = mouseButtonStates.find(button);
//     return it != mouseButtonStates.end() && it->second == KeyState::Held;
// }
//
// bool InputManager::isMouseButtonReleased(Uint8 button) const {
//     auto it = mouseButtonStates.find(button);
//     return it != mouseButtonStates.end() && it->second == KeyState::Released;
// }
//
// bool InputManager::isMouseWheelScroll(Uint8 button) const {
//     auto it = mouseButtonStates.find(button);
//     return it != mouseButtonStates.end() && it->second == KeyState::Scroll;
// }
//
// bool InputManager::isMouseButtonScrollFlip(Uint8 button) const {
//     auto it = mouseButtonStates.find(button);
//     return it != mouseButtonStates.end() && it->second == KeyState::ScrollFlip;
// }
//
int InputManager::getMouseX() const {
    return mouseX;
}

int InputManager::getMouseY() const {
    return mouseY;
}


