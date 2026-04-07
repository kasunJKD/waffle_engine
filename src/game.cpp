#include "game.h"

internal void
RenderGradient(Game_OffScreen_Buffer *Buffer)
{

                static int BlueOffset = 0;
                static int GreenOffset = 0;
                ++BlueOffset;
                GreenOffset += 2;

                u8 *Row = (u8 *)Buffer->Memory;
                for(int Y = 0; Y < Buffer->Height; ++Y)
                {
                    u32 *Pixel = (u32 *)Row;
                    for(int X = 0; X < Buffer->Width; ++X)
                    {
                        u8 Blue = (u8)(X + BlueOffset);
                        u8 Green = (u8)(Y + GreenOffset);
                        *Pixel++ = ((Green << 8) | Blue);
                    }
                    Row += Buffer->Pitch;
                }
}

extern "C" GAME_UPDATE_AND_RENDER(GameUpdateAndRender)
{
    RenderGradient(game_buffer);
}
