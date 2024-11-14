#pragma once

#include "sound.h"

struct SoundManager
{
    static map<string, Sound*> sounds;

    static void AddSound(string &path);
    static void RemoveSounds();
    static Sound* GetSound(string &name);
};

struct SoundSystem
{

};