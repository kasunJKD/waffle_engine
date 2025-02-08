#include "systemManager.h"
#include "debug.h"  // Assumes DEBUG_LOG, DEBUG_WARN, DEBUG_ERROR macros are defined here.

// Static arrays for engine and game systems.
static System engineSystems[MAX_ENGINE_SYSTEMS];
static int engineSystemCount = 0;

static System gameSystems[MAX_GAME_SYSTEMS];
static int gameSystemCount = 0;

void engine_register_engine_system(void (*init)(void), void (*update)(float dt)) {
    if (engineSystemCount >= MAX_ENGINE_SYSTEMS) {
        DEBUG_ERROR("Cannot register more engine systems!");
        return;
    }
    engineSystems[engineSystemCount].init = init;
    engineSystems[engineSystemCount].update = update;
    engineSystemCount++;
}

void engine_register_game_system(void (*init)(void), void (*update)(float dt)) {
    if (gameSystemCount >= MAX_GAME_SYSTEMS) {
        DEBUG_ERROR("Cannot register more game systems!");
        return;
    }
    gameSystems[gameSystemCount].init = init;
    gameSystems[gameSystemCount].update = update;
    gameSystemCount++;
}

void engine_remove_engine_system(void (*init)(void), void (*update)(float dt)) {
    for (int i = 0; i < engineSystemCount; i++) {
        if (engineSystems[i].init == init && engineSystems[i].update == update) {
            // Shift remaining systems left.
            for (int j = i; j < engineSystemCount - 1; j++) {
                engineSystems[j] = engineSystems[j + 1];
            }
            engineSystemCount--;
            DEBUG_LOG("Removed an engine system", engineSystemCount);
            return;
        }
    }
    DEBUG_WARN("Engine system not found for removal");
}

void engine_remove_game_system(void (*init)(void), void (*update)(float dt)) {
    for (int i = 0; i < gameSystemCount; i++) {
        if (gameSystems[i].init == init && gameSystems[i].update == update) {
            // Shift remaining systems left.
            for (int j = i; j < gameSystemCount - 1; j++) {
                gameSystems[j] = gameSystems[j + 1];
            }
            gameSystemCount--;
            DEBUG_LOG("Removed a game system", gameSystemCount);
            return;
        }
    }
    DEBUG_WARN("Game system not found for removal");
}

void engine_init_systems(void) {
    for (int i = 0; i < engineSystemCount; i++) {
        if (engineSystems[i].init)
            engineSystems[i].init();
    }
    for (int i = 0; i < gameSystemCount; i++) {
        if (gameSystems[i].init)
            gameSystems[i].init();
    }
}

void engine_update_systems(float dt) {
    for (int i = 0; i < engineSystemCount; i++) {
        if (engineSystems[i].update)
            engineSystems[i].update(dt);
    }
    for (int i = 0; i < gameSystemCount; i++) {
        if (gameSystems[i].update)
            gameSystems[i].update(dt);
    }
}
