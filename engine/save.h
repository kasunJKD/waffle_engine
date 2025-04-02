#ifndef SAVE_H
#define SAVE_H
#include "entity.h"


void save_game_data(const std::string& filename, EntitySystem *es);
void load_game_data();
void save_state_data();
void load_state_data();

#endif // SAVE_H
