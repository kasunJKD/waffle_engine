#include "utils.h"

typedef struct Game_OffScreen_Buffer
{
    // NOTE(casey): Pixels are always 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
} Game_OffScreen_Buffer;

typedef struct Game_Memory
{
    i32 IsInitialised;
    
    u64 PermanentStorageSize;
    void *PermanentStorage;
    
    u64 TransientStorageSize;
    void *TransientStorage;
} Game_Memory;

#define GAME_UPDATE_AND_RENDER(name) void name(Game_OffScreen_Buffer *game_buffer, Game_Memory *game_memory)
typedef GAME_UPDATE_AND_RENDER(game_update_and_render);
GAME_UPDATE_AND_RENDER(GameUpdateAndRenderStub)
{
}

