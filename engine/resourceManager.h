#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "allocator.h"
#include "utils/hashTable.h"
#include "defines.h"
#include <cstddef>
#include <stdio.h>
#include <string>


#define MAX_RESOURCES 256

struct ResourceManager{
    hashTable resources;
    size_t count;
    Pool_Allocator::Pool* pool;

    Resource* getResourceByName(const char* name);
};


ResourceManager* createResourceManager(Pool_Allocator::Pool* pool);

int verifyResourcePath(const char* path);
void destroyResourceManager(ResourceManager* mgr);
void unloadResource(ResourceManager* mgr, Resource* resource);
Resource* load(ResourceManager* mgr, const char* p,const char* path2, ResourceType type, const char* v, int fsize=0, bool hotreloaded=false);

void reloadChangedShaders(ResourceManager* mgr, const std::string& shaderDir = "sandbox/shaders/");


#endif //REOSURCEMANAGER_H

