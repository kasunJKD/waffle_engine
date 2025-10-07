#ifndef SHADER_H
#define SHADER_H

#include "allocator.h"
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
ShaderHandle
LoadShader(Arena* perm,
           const char* vsPath,
           const char* fsPath,
           ShaderType  type);

#endif
