
#include "assetsManager.h"
#include "texture.h"

static void add_asset(AssetManager* asset_manager, const char* name, Asset input){
    AssetManager_Add(asset_manager, name, input);
}

void add_shader
(Arena* arena,
 AssetManager* asset_manager,
 const char* vertpath,
 const char* fragpath,
 const char* asset_identifer,
 ShaderType shaderType)
{
    ShaderHandle handle = LoadShader(arena, vertpath, fragpath, shaderType);
    Asset asset;
    asset.shader = handle;
    add_asset(asset_manager, asset_identifer, asset);
    
}

void add_texture 
(
 AssetManager* asset_manager,
 const char* texture_path,
 const char* asset_identifer,
 TextureType textureType) {
    TextureHandle handle = LoadTexture(texture_path, textureType);
    Asset asset;
    asset.texture = handle;
    add_asset(asset_manager, asset_identifer, asset);
}
