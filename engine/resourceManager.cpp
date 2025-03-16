#include "resourceManager.h"
#include "shader.h"
#include "textureManager.h"
#include "debug.h"
#include "utils/hashTable.h"
#include <string>

static int fileExists(const char* path) {
    FILE* file;
    errno_t err;
    if ((err = fopen_s(&file, path, "r")) == 0) {
        fclose(file);
        return 1;
    }
    return 0;
}

ResourceManager* createResourceManager(size_t arenaSize) {
    MEM::MemoryArena* r_arena = MEM::arena_create(arenaSize);
    if (!r_arena) {
        DEBUG_ERROR("Failed to create memory arena for ResourceManager.\n");
        return NULL;
    }

    // Allocate ResourceManager from the arena
    ResourceManager* mgr = (ResourceManager*)MEM::arena_alloc(r_arena, sizeof(ResourceManager));
    if (!mgr) {
        DEBUG_ERROR("Failed to allocate ResourceManager from arena.\n");
        MEM::arena_destroy(r_arena);
        return NULL;
    }

    // Initialize ResourceManager
    mgr->arena = r_arena;
    mgr->count = 0;

    // Properly allocate the hash table
    mgr->resources.arena = r_arena;  // Assign arena to hash table
    mgr->resources = *mgr->resources.createTable();

    return mgr;
}

int verifyResourcePath(const char* path){
    return fileExists(path);
}

void destroyResourceManager(ResourceManager* mgr) {
    if (!mgr) return;
    // In a bump allocator, we cannot free individual allocations.
    // Destroying the arena frees all allocated memory in one shot.
    MEM::arena_destroy(mgr->arena);
    TextureManager::Instance().clear();
}

// void unloadResource(ResourceManager* mgr, Resource* resource) {
//     if (!mgr || !resource) return;
//     
//     // Remove the resource from the manager's array by replacing it with the last element.
//     auto erasedCount = mgr->resources.free_item(resource->name);
//     if (erasedCount > 0) {
//         mgr->count = mgr->resources.size;
//     }
//     // Note: Individual memory is not freed in a bump allocator.
//     // The memory will be reclaimed only when the entire arena is reset or destroyed.
// }

Resource* load(ResourceManager* mgr, const char* path, const char* path2, ResourceType type, const char* v) {
    if (!mgr || !path)
        return NULL;

    if (!verifyResourcePath(path)) {
        DEBUG_ERROR("Resource path does not exist: %s\n", path);
        return NULL;
    }

    if (mgr->count >= MAX_RESOURCES) {
        DEBUG_ERROR("Maximum resource count reached.\n");
        return NULL;
    }

    // Allocate the Resource structure from the fixed arena.
    Resource* res = (Resource*)MEM::arena_alloc(mgr->arena, sizeof(Resource));
    if (!res) {
        DEBUG_ERROR("Failed to allocate Resource structure for: %s\n", path);
        return NULL;
    }

    res->type = type;
    res->path = path;
    res->name = v;

    switch (type) {
        case TEXTURE: {
            GLuint texture_id = TextureManager::Instance().loadTextureFromFile(path, path, GL_RGBA, GL_RGBA, 0, 0);

            // Allocate memory for texture ID from the arena and store the pointer.
            GLuint* textureIDPtr = (GLuint*)MEM::arena_alloc(mgr->arena, sizeof(GLuint));
            *textureIDPtr = texture_id;

            res->data.i = *textureIDPtr;
            
            break;
        }
        case SHADER: {
            shader sh = shader(path, path2);

            // Allocate memory for shader ID from the arena and store the pointer.
            GLuint* shaderIDPtr = (GLuint*)MEM::arena_alloc(mgr->arena, sizeof(GLuint));
            *shaderIDPtr = sh.shaderProgramId;

            res->data.i = *shaderIDPtr;
            break;
        }
        case SOUND_WAV:
        case SOUND_STREAM:
        default:
            DEBUG_ERROR("Unknown resource type for: %s\n", path);
            return NULL;
    }

    // Insert the resource into the hash table
    mgr->resources.ht_insert(v, res);

    // Retrieve the stored resource from the map
    Resource* storedResource = mgr->resources.ht_search(v);
    if (!storedResource) {
        DEBUG_ERROR("Failed to retrieve inserted resource: %s\n", v);
        return NULL;
    }

    // Update count to reflect the new number of resources in the map
    mgr->count = mgr->resources.count;

    return storedResource;
}

//@TODO for resource manager remove manuall memory management 
//use new and use std::unordered_map<class Kty, class Ty> and use names
Resource* ResourceManager::getResourceByName(const char* name) {
    DEBUG_LOG("Currently getting resource name %s\n", name);
    return resources.ht_search(name);
}


