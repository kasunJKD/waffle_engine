#include "shader.h"
#include "allocator.h"
#include <cstdlib>
#include <stdio.h>

/* ── internal helpers ──────────────────────────────────────────────── */
static char*
ReadFileIntoArena(Arena* arena, const char* path, size_t* outLen)
{
    FILE* f = NULL;
    fopen_s(&f, path, "rb");

    if (!f) { fprintf(stderr, "Couldn't open %s\n", path); return NULL; }

    fseek(f, 0, SEEK_END);
    size_t len = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);

    char* buf = (char*)ArenaPush(arena, len + 1);
    if (fread(buf, 1, len, f) != len) {
        fclose(f); return NULL;
    }
    buf[len] = '\0';
    fclose(f);

    if (outLen) *outLen = len;
    return buf;
}
static GLuint
CompileShader(GLenum stage, const char* src, const char* dbgName)
{
    if (src == NULL) {
        fprintf(stderr, "Error: Shader source is NULL (%s)\n", dbgName);
        return 0;
    }

    // Compute length explicitly
    GLsizei srclen = (GLsizei)strlen(src);
    const GLchar* sources[] = { (const GLchar*)src };

    GLuint shader = glCreateShader(stage);
    glShaderSource(shader, 1, sources, &srclen);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = (char*)malloc(len);
            glGetShaderInfoLog(shader, len, NULL, log);
            fprintf(stderr, "Shader compilation error (%s):\n%s\n", dbgName, log);
            free(log);
        } else {
            fprintf(stderr, "Shader compilation failed (%s), but no log available.\n", dbgName);
        }
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

ShaderHandle
LoadShader(Arena* perm,
           const char* vsPath,
           const char* fsPath,
           ShaderType  type)
{
    ShaderHandle h;   // always start zeroed
    size_t len;

    char* vsSrc = ReadFileIntoArena(perm, vsPath, &len);
    char* fsSrc = ReadFileIntoArena(perm, fsPath, &len);

    GLuint vs = CompileShader(GL_VERTEX_SHADER,   vsSrc, vsPath);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc, fsPath);
    if (!vs || !fs) {
        return h;
    }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    glDeleteShader(vs);
    glDeleteShader(fs);

    GLint linkOK = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &linkOK);
    if (!linkOK) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = (char*)malloc(len);
            glGetProgramInfoLog(prog, len, NULL, log);
            fprintf(stderr, "Shader link error (%s + %s):\n%s\n", vsPath, fsPath, log);
            free(log);
        }
        glDeleteProgram(prog);
        return h;
    }

    h.program  = prog;
    h.type     = type;
    h.uTexture = glGetUniformLocation(prog, "uTexture");
    h.uMVP     = glGetUniformLocation(prog, "uMVP");
    return h;
}
