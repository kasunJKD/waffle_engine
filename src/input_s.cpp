#include "input_s.h"
#include "allocator.h"  // for Arena, PushStructZero

InputManager* input_manager_create(struct Arena* arena) {
    InputManager* im = PushStructZero(arena, InputManager);
    input_manager_init(im);
    return im;
}

void input_manager_init(InputManager* im) {
    if (!im) return;

    // Clear all states (already zeroed by PushStructZero, but for clarity)
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        im->key_states[i] = KEYSTATE_NONE;
    }
    for (int i = 0; i < INPUT_MAX_MOUSE_BUTTONS; ++i) {
        im->mouse_button_states[i] = KEYSTATE_NONE;
    }

    im->mouse_x = 0;
    im->mouse_y = 0;
    im->mouse_wheel_y = 0;
}

static void input_transition_key_states(InputManager* im) {
    // Turn PRESSED -> HELD, RELEASED -> NONE for keys
    for (int i = 0; i < SDL_NUM_SCANCODES; ++i) {
        KeyState s = im->key_states[i];
        if (s == KEYSTATE_PRESSED) {
            im->key_states[i] = KEYSTATE_HELD;
        } else if (s == KEYSTATE_RELEASED) {
            im->key_states[i] = KEYSTATE_NONE;
        }
    }

    // Mouse buttons
    for (int i = 0; i < INPUT_MAX_MOUSE_BUTTONS; ++i) {
        KeyState s = im->mouse_button_states[i];
        if (s == KEYSTATE_PRESSED) {
            im->mouse_button_states[i] = KEYSTATE_HELD;
        } else if (s == KEYSTATE_RELEASED) {
            im->mouse_button_states[i] = KEYSTATE_NONE;
        } else if (s == KEYSTATE_SCROLL || s == KEYSTATE_SCROLL_FLIP) {
            // Scroll is per-frame: reset after use
            im->mouse_button_states[i] = KEYSTATE_NONE;
        }
    }

    // Wheel is per-frame
    im->mouse_wheel_y = 0;
}

void input_handle_key_event(InputManager* im, const SDL_Event* event) {
    SDL_Scancode sc = event->key.keysym.scancode;
    if (sc < 0 || sc >= SDL_NUM_SCANCODES) return;

    if (event->type == SDL_KEYDOWN && !event->key.repeat) {
        im->key_states[sc] = KEYSTATE_PRESSED;
    } else if (event->type == SDL_KEYUP) {
        im->key_states[sc] = KEYSTATE_RELEASED;
    }
}

void input_handle_mouse_event(InputManager* im, const SDL_Event* event) {
    switch (event->type) {
    case SDL_MOUSEBUTTONDOWN: {
        Uint8 btn = event->button.button;
        if (btn < INPUT_MAX_MOUSE_BUTTONS) {
            im->mouse_button_states[btn] = KEYSTATE_PRESSED;
        }
    } break;

    case SDL_MOUSEBUTTONUP: {
        Uint8 btn = event->button.button;
        if (btn < INPUT_MAX_MOUSE_BUTTONS) {
            im->mouse_button_states[btn] = KEYSTATE_RELEASED;
        }
    } break;

    case SDL_MOUSEMOTION: {
        im->mouse_x = event->motion.x;
        im->mouse_y = event->motion.y;
    } break;

    case SDL_MOUSEWHEEL: {
        im->mouse_wheel_y = event->wheel.y;
        // If you want scroll as a state on some "virtual button", you can set it here.
    } break;
    }
}

void input_reset_states(InputManager* im) {
    // Full reset if needed (not per-frame transition)
    input_manager_init(im);
}

void input_manager_update(InputManager* im, bool *is_running, Window* window) {
    (void)window; // if you don't need it here yet

    // Step 1: transition last frame states
    input_transition_key_states(im);

    // Step 2: poll SDL events
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *is_running = false;
        }

        switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP:
            input_handle_key_event(im, &event);
            break;

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEMOTION:
        case SDL_MOUSEWHEEL:
            input_handle_mouse_event(im, &event);
            break;

        default:
            break;
        }
    }
}

// ===== Query functions =====

int input_is_key_pressed(const InputManager* im, SDL_Scancode sc) {
    if (sc < 0 || sc >= SDL_NUM_SCANCODES) return 0;
    return im->key_states[sc] == KEYSTATE_PRESSED;
}

int input_is_key_held(const InputManager* im, SDL_Scancode sc) {
    if (sc < 0 || sc >= SDL_NUM_SCANCODES) return 0;
    KeyState s = im->key_states[sc];
    return s == KEYSTATE_HELD || s == KEYSTATE_PRESSED;
}

int input_is_key_released(const InputManager* im, SDL_Scancode sc) {
    if (sc < 0 || sc >= SDL_NUM_SCANCODES) return 0;
    return im->key_states[sc] == KEYSTATE_RELEASED;
}

static int mouse_index(Uint8 button) {
    if (button >= INPUT_MAX_MOUSE_BUTTONS) return -1;
    return (int)button;
}

int input_is_mouse_pressed(const InputManager* im, Uint8 button) {
    int idx = mouse_index(button);
    if (idx < 0) return 0;
    return im->mouse_button_states[idx] == KEYSTATE_PRESSED;
}

int input_is_mouse_held(const InputManager* im, Uint8 button) {
    int idx = mouse_index(button);
    if (idx < 0) return 0;
    KeyState s = im->mouse_button_states[idx];
    return s == KEYSTATE_HELD || s == KEYSTATE_PRESSED;
}

int input_is_mouse_released(const InputManager* im, Uint8 button) {
    int idx = mouse_index(button);
    if (idx < 0) return 0;
    return im->mouse_button_states[idx] == KEYSTATE_RELEASED;
}

// For now these are stubs – you can decide what Scroll/ScrollFlip means
int input_is_mouse_scroll_flip(const InputManager* im, Uint8 button) {
    (void)im; (void)button;
    return 0;
}

int input_is_mouse_wheel_scroll(const InputManager* im) {
    return im->mouse_wheel_y != 0;
}

int input_get_mouse_x(const InputManager* im) {
    return im->mouse_x;
}

int input_get_mouse_y(const InputManager* im) {
    return im->mouse_y;
}

int input_get_mouse_wheel_y(const InputManager* im) {
    return im->mouse_wheel_y;
}
