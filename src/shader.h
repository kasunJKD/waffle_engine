#ifndef SHADER_H
#define SHADER_H

#include "glad/glad.h"

enum ShaderType {
    TEXT_SHADER,
};

struct ShaderHandle {
    GLuint program;
    ShaderType type;

    GLint uTexture;
    GLint uMVP;
};



#endif
