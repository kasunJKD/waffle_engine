#pragma once

#include "textureManager.h"
#include <string>
#include <iostream>
#include <unordered_map>
#include <type_traits>

enum ResourceType {
    TEXTURE
};

struct ResourceManager {
    void addResource(const std::string& filepath, const std::string& name, ResourceType type);

    template <typename T>
    T getResource(const std::string& name, ResourceType type) const {
        switch (type) {
            case ResourceType::TEXTURE: {
                const auto& textures = TextureManager::Instance().textures;
                auto it = textures.find(name);
                if (it != textures.end()) {
                    return static_cast<T>(it->second); // Ensure T is compatible with the texture type
                }
                std::cerr << "Texture not found: " << name << std::endl;
                break;
            }
            default:
                std::cerr << "Unsupported resource type!" << std::endl;
                break;
        }

        // Handle missing resources
        if constexpr (std::is_pointer<T>::value) {
            return nullptr; // Return nullptr for pointer types
        } else {
            return T(); // Return default-constructed value for other types
        }
    }

    void clearResources();
};
