#include "resourceManager.h"
#include "allocator.h"
#include "defines.h"
#include "font.h"
#include "shader.h"
#include "textureManager.h"
#include "debug.h"
#include "utils/hashTable.h"
#include <string>
#include <filesystem>

std::unordered_map<std::string, std::filesystem::file_time_type> shaderTimestamps;

static int fileExists(const char* path) {
    FILE* file;
    errno_t err;
    if ((err = fopen_s(&file, path, "r")) == 0) {
        fclose(file);
        return 1;
    }
    return 0;
}

ResourceManager* createResourceManager(Pool_Allocator::Pool* pool) {
     if (!pool) {
        DEBUG_ERROR("Pool is not initialized for ResourceManager.\n");
        return nullptr;
    }

    ResourceManager* mgr = (ResourceManager*)Pool_Allocator::pool_alloc(pool);
    if (!mgr) {
        DEBUG_ERROR("Failed to allocate ResourceManager from pool.\n");
        return nullptr;
    }

    mgr->pool = pool;
    mgr->count = 0;
    mgr->resources = *mgr->resources.createTable(mgr->pool);

    return mgr;   
}

int verifyResourcePath(const char* path){
    return fileExists(path);
}

void destroyResourceManager(ResourceManager* mgr) {
    if (!mgr) return;
    TextureManager::Instance().clear();
    Pool_Allocator::pool_free_all(mgr->pool);
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

Resource* load(ResourceManager* mgr, const char* p, const char* path2, ResourceType type, const char* v, int fsize, bool hotreloaded) {
    if (!mgr || !p)
        return NULL;

    if (!verifyResourcePath(p) && !hotreloaded) {
        DEBUG_ERROR("Resource path does not exist: %s\n", p);
        return NULL;
    }

    if (mgr->count >= MAX_RESOURCES) {
        DEBUG_ERROR("Maximum resource count reached.\n");
        return NULL;
    }

    // Allocate the Resource structure from the fixed arena.
     Resource* res = (Resource*)Pool_Allocator::pool_alloc(mgr->pool);
    if (!res) {
        DEBUG_ERROR("Failed to allocate Resource structure for: %s\n", p);
        return NULL;
    }

    res->type = type;
    res->path = p;
    res->name = v;

    switch (type) {
        case TEXTURE: {
            GLuint texture_id = TextureManager::Instance().loadTextureFromFile(p, p, GL_RGBA, GL_RGBA, 0, 0);
            res->data.i = texture_id;
            break;
        }
        case SHADER: {
            shader sh(p, path2);
            if(hotreloaded) {
                Resource* oldRes = mgr->getResourceByName(v);
                if (!oldRes) {
                    DEBUG_ERROR("Hot reload failed: shader not found: %s \n", v);
                    return nullptr;
                }
                oldRes->data.i = sh.shaderProgramId;
                res = oldRes;
            } else {
                res->data.i = sh.shaderProgramId;
            }
            break; 
        }
        case SOUND_WAV:
        case SOUND_STREAM:
        case FONT: {
            FontManager* fontManager = FontManager::GetInstance();
            fontManager->LoadFont(p, fsize);
            res->data.i = fontManager->fontTexture_1;
            break;
        }
        default:
            DEBUG_ERROR("Unknown resource type for: %s\n", p);
            return NULL;
    }

    mgr->resources.ht_insert(v, res);
    mgr->count = mgr->resources.count;

    return mgr->resources.ht_search(v);
}

//@TODO for resource manager remove manuall memory management 
//use new and use std::unordered_map<class Kty, class Ty> and use names
Resource* ResourceManager::getResourceByName(const char* name) {
    return resources.ht_search(name);
}

void reloadChangedShaders(ResourceManager* mgr, const std::string& shaderDir) {
    namespace fs = std::filesystem;

    for (const auto& entry : fs::directory_iterator(shaderDir)) {
        const auto& path = entry.path();
        if (!entry.is_regular_file()) continue;

        std::string ext = path.extension().string();
        if (ext != ".vert" && ext != ".frag") continue;

        std::string shaderName = path.stem().string(); // e.g., "quad" from "quad.vert"
        std::string baseName = shaderDir + shaderName;

        std::string vertPath = baseName + ".vert";
        std::string fragPath = baseName + ".frag";

        if (!fs::exists(vertPath) || !fs::exists(fragPath))
            continue; // both files must exist

        auto vertTime = fs::last_write_time(vertPath);
        auto fragTime = fs::last_write_time(fragPath);

        bool changed = false;
        std::string timeKey = shaderName; // one entry for each shader pair

        if (shaderTimestamps[timeKey + "_v"] != vertTime) {
            shaderTimestamps[timeKey + "_v"] = vertTime;
            changed = true;
        }
        if (shaderTimestamps[timeKey + "_f"] != fragTime) {
            shaderTimestamps[timeKey + "_f"] = fragTime;
            changed = true;
        }

        if (changed) {
            DEBUG_LOG("Reloading shader: %s\n", shaderName.c_str());

            shader sh(vertPath.c_str(), fragPath.c_str());

            Resource* res = mgr->getResourceByName(shaderName.c_str());
            if (res) {
                if (!res) {
                    DEBUG_ERROR("Hot reload failed: shader not found: %s\n", shaderName.c_str());
                    DEBUG_ASSERT(res, "hot shader reload. old resource invalid");
                }

                GLuint* shaderIDPtr = (GLuint*)&res->data.i;
                *shaderIDPtr = sh.shaderProgramId;
            }
        }
    }
}
