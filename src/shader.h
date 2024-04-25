#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <fstream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <glad/glad.h>



using namespace std;

struct shader {
public:

    GLuint shaderProgramId;

    shader(const char* vertexSrc, const char* fragmentSrc);
    void bind();
    void unbind();

    void setVec3(const char* uniformName,float v0, float v1, float v2);
    void setVec4(const char* uniformName,float v0, float v1, float v2, float v3);
    void setMat4(const char* uniformName, const float* value);
    void setFloat(const char* uniformName,const float* value);
private:
    GLuint vertexShaderId;
    GLuint fragmentShaderId;

    GLuint get_shader(GLenum, const char* filename);
    GLuint compile_shader(GLenum type, GLsizei, const char **sources);
    GLuint program_check(GLuint);
    const char * read_shader_file(const char* filename);
};
