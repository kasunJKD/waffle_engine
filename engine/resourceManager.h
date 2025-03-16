#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "allocator.h"
#include "utils/hashTable.h"
#include "defines.h"
#include <cstddef>
#include <stdio.h>


#define MAX_RESOURCES 256

struct ResourceManager{
    hashTable resources;
    size_t count;
    MEM::MemoryArena* arena;

    Resource* getResourceByName(const char* name);
};


ResourceManager* createResourceManager(size_t arenaSize);

int verifyResourcePath(const char* path);
void destroyResourceManager(ResourceManager* mgr);
void unloadResource(ResourceManager* mgr, Resource* resource);
Resource* load(ResourceManager* mgr, const char* path,const char* path2, ResourceType type, const char* v);


#endif //REOSURCEMANAGER_H

