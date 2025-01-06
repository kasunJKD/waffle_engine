#pragma once

#include "entity.h"
#include <unordered_map>
#include <vector>

const size_t MAX_ENTITIES = 5000;

enum ComponentType {
    None = 0,
    Transform = 1 << 0,
    Shape_Rect = 1 << 1,
    MetaData = 1 << 2,
    AudioEmitter = 1 << 3,
    Movables = 1 << 4,
    TiledMaps = 1 << 5
    // Add more components as needed
};

struct EntityManager {
    entity_id nextEntityID = 1;

    std::unordered_map<entity_id, TransformComponent> transforms;
    std::unordered_map<entity_id, EntityMetaDataComponent> metadata;
    std::unordered_map<entity_id, Shape_RectComponent> shape_rects; //temp
    std::unordered_map<entity_id, AudioEmitterComponent>audio_emitters;
    std::unordered_map<entity_id, MovableComponent> movables;
    std::unordered_map<entity_id, TiledMapComponent> tilemaps;

    std::vector<entity_id> entities;

    entity_id CreateEntity();
    void DestroyEntity(entity_id entity);
    bool HasComponent(entity_id entity);

    void AddTransformComponent(entity_id entity, const TransformComponent& transform);
    void AddShapeRectComponent(entity_id entity, const Shape_RectComponent& shapeRect);
    void AddEntityMetaDataComponent(entity_id entity, const EntityMetaDataComponent& metadata);
    void AddAudioEmitterComponent(entity_id entity, const AudioEmitterComponent& emitter);
    void AddMovableComponent(entity_id entity, const MovableComponent& movable);
    void AddTiledMapComponent(entity_id entity, const TiledMapComponent& tiledmap);


    uint32_t queryComponents(entity_id entity) const;

};

