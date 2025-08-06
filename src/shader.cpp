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
    GLuint s = glCreateShader(stage);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = (char*)malloc(len);
            glGetShaderInfoLog(s, len, NULL, log);
            fprintf(stderr, "Shader %s error:\n%s\n", dbgName, log);
            free(log);
        }
        glDeleteShader(s);
        return 0;
    }
    return s;
}

#ifdef DEBUG_ENABLED
static int
CheckProgram(GLuint prog, const char* name)
{
    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        GLint len = 0;
        glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
        if (len > 1) {
            char* log = (char*)malloc(len);
            glGetProgramInfoLog(prog, len, NULL, log);
            fprintf(stderr, "Program %s link error:\n%s\n", name, log);
            free(log);
        }
        return 0;
    }
    return 1;
}
#endif

/* ── public API ────────────────────────────────────────────────────── */
ShaderHandle
LoadShader(Arena* perm,
           const char* vsPath,
           const char* fsPath,
           ShaderType  type)
{
    ShaderHandle h;
    size_t len;

    char* vsSrc = ReadFileIntoArena(perm, vsPath, &len);
    char* fsSrc = ReadFileIntoArena(perm, fsPath, &len);
    if (!vsSrc || !fsSrc) { return h; }

    GLuint vs = CompileShader(GL_VERTEX_SHADER,   vsSrc, vsPath);
    GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsSrc, fsPath);
    if (!vs || !fs) { return h; }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram (prog);
    glDeleteShader(vs);
    glDeleteShader(fs);

    #ifdef DEBUG_ENABLED
        if (!CheckProgram(prog, fsPath)) { glDeleteProgram(prog); return h; }
    #endif

    h.program  = prog;
    h.type     = type;
    h.uTexture = glGetUniformLocation(prog, "uTexture");
    h.uMVP     = glGetUniformLocation(prog, "uMVP");
    return h;
}
