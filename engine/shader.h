#pragma once

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <fstream>

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <SDL.h>            // For SDL_Log
#include <iostream>          // For std::cerr, std::endl
#include <string>            // For std::string

using namespace std;

struct shader {
public:

    GLuint shaderProgramId;

    shader(const char* vertexSrc, const char* fragmentSrc);
    void bind();
    void unbind();

    void setVec3(const char* uniformName,float v0, float v1, float v2);
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
private:
    GLuint vertexShaderId;
    GLuint fragmentShaderId;

    GLuint get_shader(GLenum, const char* filename);
    GLuint compile_shader(GLenum type, GLsizei, const char **sources);
    GLuint program_check(GLuint);
    const char * read_shader_file(const char* filename);
};
