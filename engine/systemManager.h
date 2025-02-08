#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

struct System {
    void (*init)(void);
    void (*update)(float dt);
};

// Maximum number of systems per group.
#define MAX_ENGINE_SYSTEMS 16
#define MAX_GAME_SYSTEMS   16

// Register an engine-level system.
void engine_register_engine_system(void (*init)(void), void (*update)(float dt));

// Register a game-level system.
void engine_register_game_system(void (*init)(void), void (*update)(float dt));

// Remove an engine-level system.
void engine_remove_engine_system(void (*init)(void), void (*update)(float dt));

// Remove a game-level system.
void engine_remove_game_system(void (*init)(void), void (*update)(float dt));

// Call all registered system init functions (engine systems first, then game systems).
void engine_init_systems(void);

// Update all registered systems (engine systems first, then game systems).
void engine_update_systems(float dt);

#endif // SYSTEM_MANAGER_H

