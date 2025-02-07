#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include <glm/glm.hpp>

#define MAX_ENTITIES 1024

using EntityId = uint32_t;

struct Entity 
{
	EntityId id;
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

struct VelocityComponent {
    glm::vec3 velocity;
};


// Function to create a new entity.
Entity create_entity();

// Functions to assign components to an entity.

void initialize_component_storage(void* arenaMemory, size_t arenaSize);
void add_transform_component(Entity e, const TransformComponent& transform);

void add_velocity_component(Entity e, const VelocityComponent& velocity);
// Utility functions to check for and retrieve components.

bool has_transform_component(Entity e);

bool has_velocity_component(Entity e);

#endif //ENTITY_H
