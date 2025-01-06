#ifndef DEFINES_H
#define DEFINES_H
#include <vector>

// Enum for Sprite IDs
enum SpriteID {
    GROUND,
    WATER,
    PLAYER,
    SPRITE_COUNT // Always keep track of the count for safety
};

struct IVec2 {
    int x, y;
};

// Structure for a Sprite
struct Sprite {
    IVec2 atlasOffset;  // Position in the spritesheet
    IVec2 size;         // Dimensions of the sprite
    int gid;            // Global ID from Tiled
    int frameCount = 1;
    bool active = true;   // Default active to true
    bool visible = true;  // Visibility flag for the sprite
};

// A "layer" of tile data
struct Layer {
    std::vector<int> data; // Tile GIDs for this layer
    bool visible = true;   // Visibility flag for the layer
    std::string name;      // The name of the layer (optional)
};

// An individual object within an object group (from Tiled)
struct TiledObject {
    std::string name;
    std::string type;
    float x;
    float y;
    float width;
    float height;
    bool visible;
};

// An entire object group
struct ObjectGroup {
    std::string name;
    bool visible;
    std::vector<TiledObject> objects;
};

/**
 * TiledMap will hold both:
 *  - A list of tile layers
 *  - A list of object groups
 */
struct TiledMap {
    std::vector<Layer> layers;
    std::vector<ObjectGroup> objectGroups;
};

#endif
