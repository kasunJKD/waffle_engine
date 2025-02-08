#ifndef COMPONENT_MANAGER_H
#define COMPONENT_MANAGER_H

#include "entity.h"
#include <unordered_map>

// Template ComponentManager for custom components.
template <typename ComponentType>
class ComponentManager {
public:
    // Adds (or overwrites) a component for the given entity.
    void add_component(Entity::Entity entity, const ComponentType& component) {
        components[entity.id] = component;
    }
    
    // Checks whether the given entity has this component.
    bool has_component(Entity::Entity entity) const {
        return components.find(entity.id) != components.end();
    }
    
    // Retrieves a pointer to the component for the given entity.
    // Returns nullptr if the component is not found.
    ComponentType* get_component(Entity::Entity entity) {
        auto it = components.find(entity.id);
        if (it != components.end())
            return &it->second;
        return nullptr;
    }
    
    // Retrieve the internal container (if needed for iteration).
    const std::unordered_map<Entity::EntityId, ComponentType>& get_all_components() const {
        return components;
    }
    
private:
    std::unordered_map<Entity::EntityId, ComponentType> components;
};

#endif // COMPONENT_MANAGER_H
