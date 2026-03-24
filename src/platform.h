#ifndef WIN32_HANDMADE_H
#define WIN32_HANDMADE_H

#include <glad/glad.h>

struct SDL_Window 
{
    int Width;
    int Height;
};

#define BITMAP_BYTES_PER_PIXEL 4
typedef struct Game_OffScreen_Buffer
{
    // NOTE(casey): Pixels are always 32-bits wide, Memory Order BB GG RR XX
    void *Memory;
    int Width;
    int Height;
    int Pitch;
} Game_OffScreen_Buffer;

struct	SDL_Offscreen_Buffer 
{
    void *Memory;
    int Width;
    int Height;
    int Pitch;
    int BytesPerPixel;
};

struct opengl_state
{
    GLuint Program;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    GLuint ScreenTexture;
    GLint ScreenTextureUniform;
};

#endif
