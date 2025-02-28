#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include "allocator.h"
#include <glm/glm.hpp>

#define MAX_ENTITIES 1024
#define COMPONENT_LIMIT 10

using entityId = uint32_t;

struct Entity 
{
	entityId id;
	const char* name;
	void* components[COMPONENT_LIMIT];
	size_t component_count;
};

struct EntityList {
	Entity* entities[MAX_ENTITIES];
	size_t entity_count;
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


void entity_system_init(MEM::MemoryArena *arena);

Entity* create_entity(MEM::MemoryArena* arena, const char* name);

VelocityComponent* add_VelocityComponent(MEM::MemoryArena* arena, Entity* entity);

#endif //ENTITY_H
