#ifndef WIN32_HANDMADE_H
#define WIN32_HANDMADE_H

#include <glad/glad.h>

struct SDL_Window 
{
    int Width;
    int Height;
};

#define BITMAP_BYTES_PER_PIXEL 4

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
