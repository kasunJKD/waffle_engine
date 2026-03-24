#include <iostream>
#include "utils.h"
#include "platform.h"
#define SDL_MAIN_HANDLED
#include <SDL.h>

global_variable bool32 GlobalRunning;
global_variable bool32 GlobalPause;

global_variable SDL_Offscreen_Buffer GlobalBackbuffer = {};
global_variable SDL_Window *GlobalWindow = 0;
global_variable SDL_GLContext GlobalGLContext = 0;
global_variable opengl_state GlobalOpenGL = {};

internal void
SDLFatal(const char *Label)
{
    fprintf(stderr, "%s failed: %s\n", Label, SDL_GetError());
}

internal SDL_Window 
SDLGetWindowDimension(SDL_Window *Window)
{
    SDL_Window Result = {};
    SDL_GetWindowSize(Window, &Result.Width, &Result.Height);
    return Result;
}

internal void
SDLResizeBackbuffer(SDL_Offscreen_Buffer *Buffer, int Width, int Height)
{
    if(Buffer->Memory)
    {
        SDL_free(Buffer->Memory);
        Buffer->Memory = 0;
    }

    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel = 4;
    Buffer->Pitch = Width * Buffer->BytesPerPixel;
    Buffer->Memory = SDL_malloc((size_t)Buffer->Pitch * (size_t)Height);

    if(Buffer->Memory)
    {
        SDL_memset(Buffer->Memory, 0, (size_t)Buffer->Pitch * (size_t)Height);
    }
}

internal GLuint
OpenGLCompileShader(GLenum ShaderType, const char *Source)
{
    GLuint Shader = glCreateShader(ShaderType);
    glShaderSource(Shader, 1, &Source, 0);
    glCompileShader(Shader);

    GLint Success = 0;
    glGetShaderiv(Shader, GL_COMPILE_STATUS, &Success);
    if(!Success)
    {
        char InfoLog[2048];
        GLsizei Length = 0;
        glGetShaderInfoLog(Shader, sizeof(InfoLog), &Length, InfoLog);
        fprintf(stderr, "Shader compile error:\n%s\n", InfoLog);
    }

    return Shader;
}

internal GLuint
OpenGLCreateProgram(const char *VertexSource, const char *FragmentSource)
{
    GLuint VertexShader = OpenGLCompileShader(GL_VERTEX_SHADER, VertexSource);
    GLuint FragmentShader = OpenGLCompileShader(GL_FRAGMENT_SHADER, FragmentSource);

    GLuint Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glAttachShader(Program, FragmentShader);
    glLinkProgram(Program);

    GLint Success = 0;
    glGetProgramiv(Program, GL_LINK_STATUS, &Success);
    if(!Success)
    {
        char InfoLog[2048];
        GLsizei Length = 0;
        glGetProgramInfoLog(Program, sizeof(InfoLog), &Length, InfoLog);
        fprintf(stderr, "Program link error:\n%s\n", InfoLog);
    }

    glDeleteShader(VertexShader);
    glDeleteShader(FragmentShader);

    return Program;
}

internal void
OpenGLCreateScreenQuad(opengl_state *GL)
{
    const char *VertexShaderSource =
        "#version 330 core\n"
        "layout (location = 0) in vec2 inPos;\n"
        "layout (location = 1) in vec2 inUV;\n"
        "out vec2 fragUV;\n"
        "void main()\n"
        "{\n"
        "    fragUV = inUV;\n"
        "    gl_Position = vec4(inPos.xy, 0.0, 1.0);\n"
        "}\n";

    const char *FragmentShaderSource =
        "#version 330 core\n"
        "in vec2 fragUV;\n"
        "out vec4 outColor;\n"
        "uniform sampler2D screenTexture;\n"
        "void main()\n"
        "{\n"
        "    outColor = texture(screenTexture, fragUV);\n"
        "}\n";

    GL->Program = OpenGLCreateProgram(VertexShaderSource, FragmentShaderSource);
    GL->ScreenTextureUniform = glGetUniformLocation(GL->Program, "screenTexture");

    float Vertices[] =
    {
        // pos      // uv
        -1.0f, -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 1.0f, 1.0f,
        -1.0f,  1.0f, 0.0f, 1.0f,
    };

    u32 Indices[] =
    {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &GL->VAO);
    glGenBuffers(1, &GL->VBO);
    glGenBuffers(1, &GL->EBO);

    glBindVertexArray(GL->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, GL->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

internal void
OpenGLCreateBackbufferTexture(opengl_state *GL, int Width, int Height)
{
    if(GL->ScreenTexture)
    {
        glDeleteTextures(1, &GL->ScreenTexture);
        GL->ScreenTexture = 0;
    }

    glGenTextures(1, &GL->ScreenTexture);
    glBindTexture(GL_TEXTURE_2D, GL->ScreenTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        Width,
        Height,
        0,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        0
    );

    glBindTexture(GL_TEXTURE_2D, 0);
}

internal bool32
SDLInitOpenGL(SDL_Window *Window)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

#if defined(__APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG, 1);
#endif

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    GlobalGLContext = SDL_GL_CreateContext(Window);
    if(!GlobalGLContext)
    {
        SDLFatal("SDL_GL_CreateContext");
        return false;
    }

    if(SDL_GL_MakeCurrent(Window, GlobalGLContext) != 0)
    {
        SDLFatal("SDL_GL_MakeCurrent");
        return false;
    }

    if(!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        fprintf(stderr, "Failed to initialize GLAD\n");
        return false;
    }

    if(SDL_GL_SetSwapInterval(1) != 0)
    {
        fprintf(stderr, "Warning: VSync not enabled: %s\n", SDL_GetError());
    }

    printf("OpenGL Vendor   : %s\n", glGetString(GL_VENDOR));
    printf("OpenGL Renderer : %s\n", glGetString(GL_RENDERER));
    printf("OpenGL Version  : %s\n", glGetString(GL_VERSION));

    OpenGLCreateScreenQuad(&GlobalOpenGL);
    OpenGLCreateBackbufferTexture(&GlobalOpenGL, GlobalBackbuffer.Width, GlobalBackbuffer.Height);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    return true;
}

internal void
SDLDisplayBufferInWindow(SDL_Offscreen_Buffer *Buffer, int WindowWidth, int WindowHeight)
{
    glViewport(0, 0, WindowWidth, WindowHeight);

    glBindTexture(GL_TEXTURE_2D, GlobalOpenGL.ScreenTexture);
    glTexSubImage2D(
        GL_TEXTURE_2D,
        0,
        0,
        0,
        Buffer->Width,
        Buffer->Height,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        Buffer->Memory
    );

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(GlobalOpenGL.Program);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GlobalOpenGL.ScreenTexture);
    glUniform1i(GlobalOpenGL.ScreenTextureUniform, 0);

    glBindVertexArray(GlobalOpenGL.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    SDL_GL_SwapWindow(GlobalWindow);
}

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_AUDIO | SDL_INIT_TIMER) != 0)
    {
        SDLFatal("SDL_Init");
        return 1;
    }

    SDLResizeBackbuffer(&GlobalBackbuffer, 960, 540);

    GlobalWindow = SDL_CreateWindow(
        "GameJam",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        360,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN
    );

    if(!GlobalWindow)
    {
        SDLFatal("SDL_CreateWindow");
        SDL_Quit();
        return 1;
    }

    if(!SDLInitOpenGL(GlobalWindow))
    {
        SDL_DestroyWindow(GlobalWindow);
        SDL_Quit();
        return 1;
    }

    GlobalRunning = true;
    
    while (GlobalRunning)
    {
	if(!GlobalPause)
	{
	    Game_OffScreen_Buffer Buffer = {};
            Buffer.Memory = GlobalBackbuffer.Memory;
            Buffer.Width = GlobalBackbuffer.Width;
            Buffer.Height = GlobalBackbuffer.Height;
            Buffer.Pitch = GlobalBackbuffer.Pitch;

	    static int BlueOffset = 0;
            static int GreenOffset = 0;
            ++BlueOffset;
            GreenOffset += 2;

            u8 *Row = (u8 *)Buffer.Memory;
            for(int Y = 0; Y < Buffer.Height; ++Y)
            {
                u32 *Pixel = (u32 *)Row;
                for(int X = 0; X < Buffer.Width; ++X)
                {
                    u8 Blue = (u8)(X + BlueOffset);
                    u8 Green = (u8)(Y + GreenOffset);
                    *Pixel++ = ((Green << 8) | Blue);
                }
                Row += Buffer.Pitch;
            }

            SDL_Window Dimension = SDLGetWindowDimension(GlobalWindow);
            SDLDisplayBufferInWindow(&GlobalBackbuffer, Dimension.Width, Dimension.Height);
	}

    }


    if(GlobalOpenGL.Program)
    {
        glDeleteProgram(GlobalOpenGL.Program);
    }

    if(GlobalOpenGL.VAO)
    {
        glDeleteVertexArrays(1, &GlobalOpenGL.VAO);
    }

    if(GlobalOpenGL.VBO)
    {
        glDeleteBuffers(1, &GlobalOpenGL.VBO);
    }

    if(GlobalOpenGL.EBO)
    {
        glDeleteBuffers(1, &GlobalOpenGL.EBO);
    }

    if(GlobalOpenGL.ScreenTexture)
    {
        glDeleteTextures(1, &GlobalOpenGL.ScreenTexture);
    }

    if(GlobalGLContext)
    {
        SDL_GL_DeleteContext(GlobalGLContext);
    }

    if(GlobalWindow)
    {
        SDL_DestroyWindow(GlobalWindow);
    }

    if(GlobalBackbuffer.Memory)
    {
        SDL_free(GlobalBackbuffer.Memory);
    }

    SDL_Quit();
    return 0;
}
