
//SpriteManager* SpriteManager::instance = nullptr;

//load map of defined sprites
//[SpriteType: Sprite]
//get sprite pointer by SpriteType
#include "allocator.h"
#include "resourceManager.h"
#include "utils/hashTable.h"

struct SpriteManager {
    hashTable *spriteTable;
    size_t count;
    Pool_Allocator::Pool* pool;
    ResourceManager* r_mgr;

    Sprite* getSprite(const char* name); 
};

SpriteManager* createSpriteManager(Pool_Allocator::Pool* pool, ResourceManager* r_manager);
void loadSpritesMapping(SpriteManager* sp_mgr);

