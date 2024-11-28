#include "resourceManager.h"
#include <cassert>
#include <iostream>

void ResourceManager::addResource(const std::string& filepath, const std::string& name, ResourceType type) {
    if (type == TEXTURE) {
        TextureManager::Instance().loadTextureFromFile(filepath.c_str(), name);
    } else {
        std::cerr<< "Unknown ResourceType!" << std::endl;
    }
}

template <typename T> T ResourceManager::getResource(const std::string &name, ResourceType type) const
{
    switch (type) {
        case ResourceType::TEXTURE:
            auto& textures = TextureManager::Instance().textures;
            assert(textures.find(name) != textures.end());
            if (textures.find(name) != textures.end()) {
                return static_cast<T>(textures.at(name));
            }
            std::cerr << "Texture not found: " << name << std::endl;
            break;
        default:
            assert(nullptr);
            return nullptr;
    }
}

void ResourceManager::clearResources() {
    TextureManager::Instance().clear();
}
