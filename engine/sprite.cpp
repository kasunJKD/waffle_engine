#include "sprite.h"
#include "allocator.h"
#include "debug.h"

SpriteManager* createSpriteManager(Pool_Allocator::Pool* pool, ResourceManager* r_manager)
{
    if (!pool) {
        DEBUG_ERROR("Pool is not initialized for SpriteManager.\n");
        return nullptr;
    }

    DEBUG_ASSERT(sizeof(SpriteManager) <= pool->chunk_size, "Pool chunk size is too small for SpriteManager");

    SpriteManager* mgr = (SpriteManager*)Pool_Allocator::pool_alloc(pool);
    if (!mgr) {
        DEBUG_ERROR("Failed to allocate SpriteManager from pool.\n");
        return nullptr;
    }

    mgr->pool = pool;
    mgr->r_mgr = r_manager;
    mgr->count = 0;

    mgr->spriteTable = createTable(pool); // SAFE: Avoid calling method on garbage

    return mgr;   
}

void loadSpritesMapping(
SpriteManager* sp_mgr
) {

    //player sprite
    {
        Sprite* s = (Sprite*)Pool_Allocator::pool_alloc(sp_mgr->pool);
        s->texture_id = sp_mgr->r_mgr->getResourceByName("spritesheet")->data.i;
        s->frame_size= glm::vec2(32, 64);
        s->sheet_size = glm::ivec2(512, 288);  // Atlas = pixel pos
        s->pixel_offset = glm::vec2(32,48);
        s->frame_count = 1;
        s->frame_duration = 0.1;

        DEBUG_ASSERT(s!=nullptr, "player sprite is nullptr");

        sp_mgr->spriteTable->ht_insert("player", s);
        sp_mgr->count++;
    }
    {
        Sprite* s = (Sprite*)Pool_Allocator::pool_alloc(sp_mgr->pool);
        s->texture_id = sp_mgr->r_mgr->getResourceByName("spritesheet")->data.i;
        s->frame_size= glm::vec2(32, 32);
        s->sheet_size = glm::ivec2(512, 288);  // Atlas = pixel pos
        s->pixel_offset = glm::vec2(0,0);
        s->frame_count = 1;
        s->frame_duration = 0.1;

        DEBUG_ASSERT(s!=nullptr, "grass sprite is nullptr");

        sp_mgr->spriteTable->ht_insert("grass", s);
        sp_mgr->count++;
    }
    {
        Sprite* s = (Sprite*)Pool_Allocator::pool_alloc(sp_mgr->pool);
        s->texture_id = sp_mgr->r_mgr->getResourceByName("spritesheet")->data.i;
        s->frame_size= glm::vec2(32, 32);
        s->sheet_size = glm::ivec2(512, 288);  // Atlas = pixel pos
        s->pixel_offset = glm::vec2(32,0);
        s->frame_count = 1;
        s->frame_duration = 0.1;

        DEBUG_ASSERT(s!=nullptr, "water sprite is nullptr");

        sp_mgr->spriteTable->ht_insert("water", s);
        sp_mgr->count++;
    }

    //....continue others
    
}

Sprite* SpriteManager::getSprite(const char* name)
{
    Sprite* v = spriteTable->ht_search_sprite(name);
    DEBUG_ASSERT(v != nullptr, "get sprite not working");
    return v;
}

std::vector<std::pair<std::string,Sprite*>> SpriteManager::getAllSprites() const {
    std::vector<std::pair<std::string,Sprite*>> list;
    list.reserve(count);

    for (size_t i = 0; i < spriteTable->size; ++i) {
        hitem* entry = spriteTable->items[i];
        if (!entry) 
            continue;

        // Only care about Sprite* variants
        if (std::holds_alternative<Sprite*>(entry->data)) {
            Sprite* s = std::get<Sprite*>(entry->data);
            list.emplace_back(entry->key, s);
        }
    }

    return list;
}
