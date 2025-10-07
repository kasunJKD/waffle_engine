#ifndef ASSETMANAGER_H
#define ASSETMANAGER_H

#include "hashmap.h"
#include "shader.h"
#include "texture.h"
#include <cstdint>

enum AssetType {
    SHADER,
    TEXTURE,
    MESH,
};

struct Asset {
    uint32_t id;
    uint32_t flags;
    union {
        ShaderHandle shader;
        TextureHandle texture;
    };
};

struct AssetManager {
    Asset* assets;       /* dense array in gPerm            */
    size_t capacity;
    size_t count;
    ht     nameToId;     /* key = const char*, value = (void*)(uintptr_t)id */
};

static inline AssetManager
AssetManager_Create(Arena* perm, size_t maxAssets)
{
    AssetManager am;
    am.capacity = maxAssets;
    am.count    = 0;
    am.assets   = PushArrayZero(perm, Asset, maxAssets);
    am.nameToId = ht_create(perm, maxAssets*2);
    return am;
}

/* register asset & name, returns dense id */
static inline uint32_t
AssetManager_Add(AssetManager* am, const char* name, Asset asset)
{
    uint32_t id = (uint32_t)am->count++;
    assert(id < am->capacity);
    asset.id = id;
    am->assets[id] = asset;
    ht_put(&am->nameToId, name, id);
    return id;
}

static inline Asset*
AssetManager_GetById(AssetManager* am, uint32_t id)
{
    return (id < am->count) ? &am->assets[id] : NULL;
}

static inline Asset*
AssetManager_GetByName(AssetManager* am, const char* name)
{
    uintptr_t v = ht_get(&am->nameToId, name);
    return &am->assets[v];
}

void add_shader
(Arena* arena,
 AssetManager* asset_manager,
 const char* vertpath,
 const char* fragpath,
 const char* asset_identifer,
 ShaderType shaderType);

void add_texture 
(
 AssetManager* asset_manager,
 const char* texture_path,
 const char* asset_identifer,
 TextureType textureType);

#endif
