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


void ResourceManager::clearResources() {
    TextureManager::Instance().clear();
}
