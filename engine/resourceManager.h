#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "allocator.h"
#include <cstddef>
#include <stdio.h>
enum ResourceType {
    TEXTURE,
    SHADER ,
    SOUND_WAV,
    SOUND_STREAM
};

struct Resource {
    ResourceType type;
    const char* path;
    void* data;
    size_t size;
};

#define MAX_RESOURCES 256

struct ResourceManager{
    Resource* resources[MAX_RESOURCES];
    size_t count;
    MEM::MemoryArena* arena;
};


ResourceManager* createResourceManager(size_t arenaSize);

int verifyResourcePath(const char* path);
void destroyResourceManager(ResourceManager* mgr);
void unloadResource(ResourceManager* mgr, Resource* resource);
Resource* load(ResourceManager* mgr, const char* path, ResourceType type);


#endif //REOSURCEMANAGER_H

