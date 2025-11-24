#ifndef INPUT_S_H
#define INPUT_S_H

#include "window.h"
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>

// Forward-declare Arena so we don't depend on allocator headers here.
struct Arena;

typedef enum KeyState {
    KEYSTATE_NONE = 0,
    KEYSTATE_PRESSED,
    KEYSTATE_HELD,
    KEYSTATE_RELEASED,
    KEYSTATE_SCROLL,
    KEYSTATE_SCROLL_FLIP
} KeyState;

// NOTE: We use SDL scancodes so we can index directly into an array.
// SDL_NUM_SCANCODES is the max scancode count.
#define INPUT_MAX_MOUSE_BUTTONS 8

typedef struct InputManager {
    KeyState key_states[SDL_NUM_SCANCODES];
    KeyState mouse_button_states[INPUT_MAX_MOUSE_BUTTONS];

    int mouse_x;
    int mouse_y;
    int mouse_wheel_y;
} InputManager;

// Creation / init
InputManager* input_manager_create(struct Arena* arena);
void input_manager_init(InputManager* im);

// Per-frame update (polls SDL events, updates states, mouse, etc.)
void input_manager_update(InputManager* im, bool *is_running, Window* window);

// Key queries (use SDL_Scancode, e.g. SDL_SCANCODE_W)
int input_is_key_pressed (const InputManager* im, SDL_Scancode scancode);
int input_is_key_held    (const InputManager* im, SDL_Scancode scancode);
int input_is_key_released(const InputManager* im, SDL_Scancode scancode);

// Mouse button queries (1 = left, 2 = middle, 3 = right, etc.)
int input_is_mouse_pressed     (const InputManager* im, Uint8 button);
int input_is_mouse_held        (const InputManager* im, Uint8 button);
int input_is_mouse_released    (const InputManager* im, Uint8 button);
int input_is_mouse_scroll_flip (const InputManager* im, Uint8 button);
int input_is_mouse_wheel_scroll(const InputManager* im);

// Mouse / wheel getters
int input_get_mouse_x(const InputManager* im);
int input_get_mouse_y(const InputManager* im);
int input_get_mouse_wheel_y(const InputManager* im);

// Internal helpers (you can keep these in the .c if you want them private)
void input_handle_key_event  (InputManager* im, const SDL_Event* event);
void input_handle_mouse_event(InputManager* im, const SDL_Event* event);
void input_reset_states      (InputManager* im);


#endif // INPUT_S_H

