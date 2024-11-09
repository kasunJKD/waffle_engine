#include "entityManager.h"

entity_id EntityManager::CreateEntity()
{
    entity_id entityID = nextEntityID++;
    entities.push_back(entityID);
    AddEntityMetaDataComponent(entityID, {true});
    return entityID;
}

void EntityManager::DestroyEntity(entity_id entity)
{
    entities.erase(std::remove(entities.begin(), entities.end(), entity), entities.end());

    // Remove components associated with the entity
    transforms.erase(entity);
    shape_rects.erase(entity);
}

uint32_t EntityManager::queryComponents(entity_id entity) const {
    uint32_t componentMask = ComponentType::None;

    // Check each component type and set the corresponding bit if present
    if (transforms.find(entity) != transforms.end()) {
        componentMask |= ComponentType::Transform;
    }
    if (shape_rects.find(entity) != shape_rects.end()) {
        componentMask |= ComponentType::Shape_Rect;
    }
    if (metadata.find(entity) != metadata.end()) {
        componentMask |= ComponentType::MetaData;
    }

    return componentMask;
}

// Check if an entity has any components (useful for checking if the entity is "valid" in ECS terms)
bool EntityManager::HasComponent(entity_id entity) {
    return transforms.find(entity) != transforms.end() ||
           shape_rects.find(entity) != shape_rects.end();
}

// Add a TransformComponent to an entity
void EntityManager::AddTransformComponent(entity_id entity, const TransformComponent& transform) {
    transforms[entity] = transform;
}

// Add a RenderComponent to an entity
void EntityManager::AddShapeRectComponent(entity_id entity, const Shape_RectComponent& shapeRect) {
    shape_rects[entity] = shapeRect;
}

//add metadata component to a entity by default
void EntityManager::AddEntityMetaDataComponent(entity_id entity, const EntityMetaDataComponent& metadata_)
{
    metadata[entity] = metadata_;
}
