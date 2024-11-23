#include "sceneManager.h"
#include <fstream>
#include <iostream>

#include <json.hpp>

using json = nlohmann::json;

SceneManager::SceneManager(EntityManager* entityManager) : entityManager(entityManager) {}

void SceneManager::createScene(const std::string& name) {
    if (scenes.find(name) == scenes.end()) {
        Scene scene = {};
        scene.name = name;
        scenes[name] = scene;

        // Create a new JSON file for the scene
        json sceneData = {
            {"name", scene.name},
            {"entities", json::array()}
        };

        std::ofstream file(name + ".json", std::ios::out);
        if (file.is_open()) {
            file << sceneData.dump(4); // Write formatted JSON
            file.close();
            std::cout << "Scene file created: " << name << ".json" << std::endl;
        } else {
            std::cerr << "Failed to create scene file: " << name << ".json" << std::endl;
        }
    } else {
        std::cerr << "Scene " << name << " already exists!" << std::endl;
    }
}

void SceneManager::loadScene(const std::string& name) {
    if (scenes.find(name) == scenes.end()) {
        std::cerr << "Scene " << name << " not found!" << std::endl;
        return;
    }

    currentScene = &scenes[name];
    entityManager->entities.clear();

    std::ifstream file(name + ".json", std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open scene file: " << name << ".json" << std::endl;
        return;
    }

    json sceneData;
    file >> sceneData;
    file.close();

    for (const auto& entityData : sceneData["entities"]) {
        entity_id entity = entityManager->CreateEntity();
        currentScene->entities.push_back(entity);

        if (entityData.contains("transform")) {
            auto& transformData = entityData["transform"];
            TransformComponent transform;
            transform.position = glm::vec3(
                transformData["position"][0],
                transformData["position"][1],
                transformData["position"][2]
            );
            transform.rotation = transformData["rotation"];
            transform.scale = glm::vec2(
                transformData["scale"][0],
                transformData["scale"][1]
            );
            entityManager->AddTransformComponent(entity, transform);
        }
        // Add other components as needed
    }
}

void SceneManager::saveScene(const std::string& name) {
    if (scenes.find(name) == scenes.end()) {
        std::cerr << "Scene " << name << " not found!" << std::endl;
        return;
    }

    json sceneData;
    for (entity_id entity : scenes[name].entities) {
        json entityData;
        if (entityManager->transforms.find(entity) != entityManager->transforms.end()) {
            auto& transform = entityManager->transforms[entity];
            entityData["transform"] = {
                {"position", {transform.position.x, transform.position.y, transform.position.z}},
                {"rotation", transform.rotation},
                {"scale", {transform.scale.x, transform.scale.y}}
            };
        }
        // Add other components as needed
        sceneData["entities"].push_back(entityData);
    }

    std::ofstream file(name + ".json", std::ios::out);
    if (file.is_open()) {
        file << sceneData.dump(4);
        file.close();
    }
}

void SceneManager::switchScene(const std::string& name) {
    if (currentScene) {
        saveScene(currentScene->name);
    }
    loadScene(name);
}

void SceneManager::update(float deltaTime) {
    if (currentScene) {
        // Update scene-specific logic if needed
        handleSceneTriggers();
    }
}

void SceneManager::render() {
    if (currentScene) {
        // Render scene-specific entities if needed
    }
}

void SceneManager::handleSceneTriggers() {
    // Handle triggers to switch scenes (e.g., collisions, player actions)
}

Scene* SceneManager::getCurrentScene() {
    return currentScene;
}

void SceneManager::addEntityToScene(entity_id entity) {
    if (!currentScene) {
        std::cerr << "No scene is currently active. Cannot add entity." << std::endl;
        return;
    }

    // Add entity to the current scene's entity list
    currentScene->entities.push_back(entity);

    // Serialize the entity's components into JSON
    json entityData;
    entityData["entity_id"] = entity;

    if (entityManager->transforms.find(entity) != entityManager->transforms.end()) {
        auto& transform = entityManager->transforms[entity];
        entityData["transform"] = {
            {"position", {transform.position.x, transform.position.y, transform.position.z}},
            {"rotation", transform.rotation},
            {"scale", {transform.scale.x, transform.scale.y}}
        };
    }

    if (entityManager->shape_rects.find(entity) != entityManager->shape_rects.end()) {
        auto& rect = entityManager->shape_rects[entity];
        entityData["shape_rect"] = {
            {"size", {rect.size.x, rect.size.y}},
            {"color", {rect.color.r, rect.color.g, rect.color.b, rect.color.a}},
            {"filled", rect.filled}
        };
    }

    if (entityManager->audio_emitters.find(entity) != entityManager->audio_emitters.end()) {
        auto& audio = entityManager->audio_emitters[entity];
        entityData["audio_emitter"] = {
            {"soundName", audio.soundName},
            {"isPlaying", audio.isPlaying},
            {"loop", audio.loop},
            {"volume", audio.volume},
            {"pitch", audio.pitch}
        };
    }

    // Add the serialized entity data to the scene JSON
    std::string sceneFileName = currentScene->name + ".json";

    json sceneData;
    std::ifstream sceneFile(sceneFileName);
    if (sceneFile.is_open()) {
        sceneFile >> sceneData;
        sceneFile.close();
    }

    sceneData["entities"].push_back(entityData);

    // Write the updated scene data back to the JSON file
    std::ofstream outFile(sceneFileName);
    if (outFile.is_open()) {
        outFile << sceneData.dump(4);
        outFile.close();
        std::cout << "Entity added to scene: " << currentScene->name << std::endl;
    } else {
        std::cerr << "Failed to write to scene file: " << sceneFileName << std::endl;
    }
}

void SceneManager::removeEntityFromScene(entity_id entity) {
    if (!currentScene) {
        std::cerr << "No scene is currently active. Cannot remove entity." << std::endl;
        return;
    }

    // Check if the entity exists in the current scene
    auto& entities = currentScene->entities;
    auto it = std::find(entities.begin(), entities.end(), entity);
    if (it == entities.end()) {
        std::cerr << "Entity not found in the current scene: " << entity << std::endl;
        return;
    }

    // Remove the entity from the scene's entity list
    entities.erase(it);

    // Update the scene JSON file
    std::string sceneFileName = currentScene->name + ".json";

    json sceneData;
    std::ifstream sceneFile(sceneFileName);
    if (sceneFile.is_open()) {
        sceneFile >> sceneData;
        sceneFile.close();
    } else {
        std::cerr << "Failed to open scene file: " << sceneFileName << std::endl;
        return;
    }

    // Find and remove the entity data from the JSON
    if (sceneData.contains("entities")) {
        auto& jsonEntities = sceneData["entities"];
        for (auto it = jsonEntities.begin(); it != jsonEntities.end(); ++it) {
            if ((*it).contains("transform") && (*it)["entity_id"] == entity) {
                jsonEntities.erase(it);
                break;
            }
        }
    }

    // Save the updated scene data back to the file
    std::ofstream outFile(sceneFileName);
    if (outFile.is_open()) {
        outFile << sceneData.dump(4);
        outFile.close();
        std::cout << "Entity removed from scene: " << currentScene->name << std::endl;
    } else {
        std::cerr << "Failed to write to scene file: " << sceneFileName << std::endl;
    }

    // Optionally remove entity data from the EntityManager
    entityManager->DestroyEntity(entity);
}
