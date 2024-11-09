#ifndef ENTITY_H
#define ENTITY_H

#include <glm/glm.hpp>
#include <cstdint>

using entity_id = uint32_t;

struct EntityMetaDataComponent {
    bool active;
};

struct TransformComponent {
    glm::vec2 position;
    float rotation;   // In degrees
    glm::vec2 scale;
};

struct Shape_RectComponent {
    glm::vec2 size{50.0f, 50.0f};     // Width and height of the rectangle
    glm::vec4 color{1.0f, 1.0f, 1.0f, 1.0f}; // RGBA color for the rectangle
    bool filled = true;               // Flag to determine if the rectangle is filled or outlined
};

#endif // ENTITY_H