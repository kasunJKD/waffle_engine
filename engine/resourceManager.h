#pragma once

#include "textureManager.h"
#include <string>

enum ResourceType {
    TEXTURE
};

struct ResourceManager 
{
    void addResource(const std::string& filepath, const std::string& name, ResourceType type);
    template <typename T>
    T getResource(const std::string& name, ResourceType type) const;

    void clearResources();
};
