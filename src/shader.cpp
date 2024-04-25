#include "shader.h"



shader::shader(const char* vertexSrc, const char* fragmentSrc)
{
    
   vertexShaderId = get_shader(GL_VERTEX_SHADER, vertexSrc);
   fragmentShaderId = get_shader(GL_FRAGMENT_SHADER, fragmentSrc);

   shaderProgramId = glCreateProgram();

   glAttachShader(shaderProgramId, vertexShaderId);
   glAttachShader(shaderProgramId, fragmentShaderId);

   glLinkProgram(shaderProgramId);

   glReleaseShaderCompiler();

     // Check for linking errors
    GLint success;
    GLchar infoLog[512];
    glGetProgramiv(shaderProgramId, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgramId, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
}

void shader::bind()
{
    glUseProgram(shaderProgramId);
}

void shader::unbind()
{
  glDetachShader(shaderProgramId, vertexShaderId);
  glDetachShader(shaderProgramId, fragmentShaderId);
  glDeleteShader(vertexShaderId);
  glDeleteShader(fragmentShaderId);
  glDeleteProgram(shaderProgramId);
}

GLuint shader::get_shader(GLenum eShaderType, const char* filename)

{
    const char *shaderSource=read_shader_file(filename);
    GLuint shader = compile_shader(eShaderType, 1, &shaderSource);
    return shader;
}


GLuint shader::compile_shader(GLenum type, GLsizei nsources, const char **sources)
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

GLuint shader::program_check(GLuint program)
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

const char* shader::read_shader_file(const char* filename) {
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

void shader::setVec3(const char* uniformName, float v0, float v1, float v2)
{
    unsigned int location = glGetUniformLocation(shaderProgramId, uniformName);
    glUniform3f(location,v0,v1,v2);
}
void shader::setMat4(const char* uniformName, const float* value)
{
    unsigned int location = glGetUniformLocation(shaderProgramId, uniformName);
    glUniformMatrix4fv(location,1,false, value);
}
void shader::setFloat(const char* uniformName,const float* value)
{
    unsigned int location = glGetUniformLocation(shaderProgramId, uniformName);
    glUniform1fv(location,1, value);
}

void shader::setVec4(const char* uniformName,float v0, float v1, float v2, float v3)
{
    unsigned int location = glGetUniformLocation(shaderProgramId, uniformName);
    glUniform4f(location,v0,v1, v2, v3);
}



