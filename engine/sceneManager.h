#pragma once

#include "entityManager.h"
#include <string>
#include <unordered_map>

struct Scene {
    std::string name;
    std::vector<entity_id> entities;
};

struct SceneManager {
    std::unordered_map<std::string, Scene> scenes;
    Scene* currentScene = nullptr;
    EntityManager* entityManager;

    SceneManager(EntityManager* entityManager);

    void createScene(const std::string& name);
    void loadScene(const std::string& name);
    void saveScene(const std::string& name);
    void switchScene(const std::string& name);

    void update(float deltaTime);
    void render();
    void handleSceneTriggers();

    Scene* getCurrentScene();
    void addEntityToScene(entity_id entity);
    void removeEntityFromScene(entity_id entity);

};
