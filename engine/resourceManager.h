#pragma once

#include "shader.h"
#include <string>
#include <unordered_map>


struct Resources
{
    //@TODO add all shaders, meshes, textures, sounds and create its own initialisation and assign to array ? maybe ? store its pointers ? get a pointer by name ?
    //std::unordered_map<std::string name, shader* shader> shaders;
    std::unordered_map<std::string, GLuint> textures;
};

enum ResourceType {
    TEXTURE,
    SHADER
};

struct ResourceManager 
{
    ResourceManager(const Resources& resources);
    void addResources (std::string& filepath, std::string& name); 
    
    template <typename T>
    T getResource(const std::string& name, ResourceType type) const;
};
