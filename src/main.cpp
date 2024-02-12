#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <fstream>
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>

using namespace std;

typedef int32_t i32;
typedef uint32_t u32;
typedef uint32_t b32;

#define WinWidth 1000
#define WinHeight 1000

GLuint shading_program;
GLuint vertexShader;
GLuint fragmentShader;

#define GL_GREY .5, .5, .5, 1

GLuint GetShader(GLenum, const char *);
GLuint compile_shader(GLenum type, GLsizei, const char **);
GLuint program_check(GLuint);
const char * read_file(const char *);

int main (int argc, char* argv[])
{

  SDL_Init(SDL_INIT_EVERYTHING);
  Uint32 WindowFlags = SDL_WINDOW_HIDDEN|SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
  SDL_Window *Window = SDL_CreateWindow("", 0, 0, 0, 0, WindowFlags);
  SDL_GLContext Context = SDL_GL_CreateContext(Window);
  SDL_GL_MakeCurrent(Window, Context);

   SDL_SetWindowSize(Window, WinWidth, WinWidth);
   SDL_SetWindowPosition(Window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
   SDL_SetWindowTitle(Window, "Context 4.6 with GLAD");
   SDL_ShowWindow(Window);

   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

   gladLoadGLLoader(SDL_GL_GetProcAddress);

   glClearColor(GL_GREY);

   vertexShader   = GetShader(GL_VERTEX_SHADER, "shaders/quad.vert");
   fragmentShader = GetShader(GL_FRAGMENT_SHADER, "shaders/quad.frag");

   shading_program = glCreateProgram();

   glAttachShader(shading_program, vertexShader);
   glAttachShader(shading_program, fragmentShader);

   glLinkProgram(shading_program);

   glReleaseShaderCompiler();
   glUseProgram(shading_program);
    
  
  b32 Running = 1;
  b32 FullScreen = 0;
  while (Running)
  {
    SDL_Event Event;
    while (SDL_PollEvent(&Event))
    {
      if (Event.type == SDL_KEYDOWN)
      {
        switch (Event.key.keysym.sym)
        {
          case SDLK_ESCAPE:
            Running = 0;
            break;
          case 'f':
            FullScreen = !FullScreen;
            if (FullScreen)
            {
              SDL_SetWindowFullscreen(Window, WindowFlags | SDL_WINDOW_FULLSCREEN_DESKTOP);
            }
            else
            {
              SDL_SetWindowFullscreen(Window, WindowFlags);
            }
            break;
          default:
            break;
        }
	
      }
      else if (Event.type == SDL_QUIT)
      {
        Running = 0;
      }
    }

    glViewport(0, 0, WinWidth, WinHeight);
    glClearColor(1.f, 0.f, 1.f, 0.f);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(Window);
  }

  glDetachShader(shading_program, vertexShader);
  glDetachShader(shading_program, fragmentShader);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteProgram(shading_program);

  SDL_GL_DeleteContext(Context);
  SDL_DestroyWindow(Window);
  SDL_Quit();
  return 0;
}

GLuint GetShader(GLenum eShaderType, const char *filename)
{

    const char *shaderSource=read_file(filename);
    GLuint shader = compile_shader(eShaderType, 1, &shaderSource);
    return shader;

}

GLuint compile_shader(GLenum type, GLsizei nsources, const char **sources)
{

    GLuint  shader;
    GLint   success, len;
    GLsizei i, srclens[nsources];

    for (i = 0; i < nsources; ++i)
        srclens[i] = (GLsizei)strlen(sources[i]);

    shader = glCreateShader(type);
    glShaderSource(shader, nsources, sources, srclens);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log;
            log = (char *)malloc(len);
            glGetShaderInfoLog(shader, len, NULL, log);
            fprintf(stderr, "%s\n\n", log);
            free(log);
        }
        SDL_Log("Error compiling shader.\n");
    }
    SDL_Log("shader: %u", shader);
    return shader;
}

GLuint program_check(GLuint program)
{
    //Error Checking
    GLint status;
    glValidateProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (!status) {
        GLint len;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char *log;
            log = (char *)malloc(len);
            glGetProgramInfoLog(program, len, &len, log);
            fprintf(stderr, "%s\n\n", log);
            free(log);
        }
        SDL_Log("Error linking shader default program.\n");
        return GL_FALSE;
    }
    return GL_TRUE;
}

const char* read_file(const char* filename) {
    // Open the file in binary mode
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Could not open the file!" << endl;
        return nullptr; // Return nullptr if file couldn't be opened
    }

    // Determine the file length
    file.seekg(0, ios::end);
    size_t length = file.tellg();
    file.seekg(0, ios::beg);

    // Allocate memory for file content
    char* buffer = new char[length + 1]; // +1 for null terminator

    // Read the file
    file.read(buffer, length);
    buffer[length] = '\0'; // Null-terminate the string

    // Close the file
    file.close();

    return buffer;
}
