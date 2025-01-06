#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <cstdint>
#include <string>
#include <iostream> // For std::cerr and std::endl
#include <AL/al.h>
#include <AL/alc.h>
#include "defines.h"

using entity_id = uint32_t;

struct EntityMetaDataComponent
{
    bool active;
};

struct TransformComponent
{
    glm::vec3 position;
    float rotation;   // In degrees
    glm::vec2 scale;

    //used in audio
    glm::vec3 forward;
    glm::vec3 up;
};

struct Shape_RectComponent
{
    glm::vec2 size{50.0f, 50.0f};     // Width and height of the rectangle
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f}; // RGBA color for the rectangle
    bool filled = true;               // Flag to determine if the rectangle is filled or outlined
};

//audio based components
//1.entity will have emitter component and update from it
//2.manual sound play will be posible without a entity for UI
struct AudioEmitterComponent
{
    std::string soundName;         // Name of the sound in SoundManager
    ALuint source;                 // OpenAL source ID
    bool isPlaying;                // Playback state
    bool loop;                     // Looping flag
    float volume;                  // Volume (0.0f to 1.0f)
    float pitch;                   // Pitch (1.0f is normal)
};

struct MovableComponent {
    float speed;  // Movement speed
    glm::vec3 direction; // Current movement direction
};

struct TiledMapComponent {
    TiledMap tilemap;
};

#endif // ENTITY_H
