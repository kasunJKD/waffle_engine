#include "glad/glad.h"
#include "glm/gtc/type_ptr.hpp"
#include "glrenderSystem.h"
#include <iostream>
#include <string>


void CheckGLError(const std::string& message) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr<< "OpenGL Error (" << message << "): " << err << std::endl;
    }
}

void initRenderSystem(RenderSystem* rs, ResourceManager* rm) {
    rs->count = 0;
    rs->resManager = rm;
    
}

void createRenderComponent(RenderSystem* rs, 
                     Entity* e)
{
    if (rs->count >= MAX_RENDER_COMPONENTS) {
        fprintf(stderr, "RenderSystem: too many components!\n");
    }

    rs->components[rs->count++] = e;
}

void InitWorldQuad(Entity* rc) {
    GLuint quadVBO;
    float quadVertices[] = {
        // position     // texCoord
        0.0f,    0.0f,    0.0f, 0.0f,
        3072.0f, 0.0f,    1.0f, 0.0f,
        3072.0f, 1728.0f, 1.0f, 1.0f,

        0.0f,    0.0f,    0.0f, 0.0f,
        3072.0f, 1728.0f, 1.0f, 1.0f,
        0.0f,    1728.0f, 0.0f, 1.0f
    };

    glGenVertexArrays(1, &rc->VAO);
    CheckGLError("glGenVertexArrays");
    glGenBuffers(1, &quadVBO);
    CheckGLError("glGenBuffers");

    glBindVertexArray(rc->VAO);
    CheckGLError("glbindvertexarray");

    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    CheckGLError("glbindbuffer");
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    // Position attribute (2D quad)
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Texture coordinate attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void RenderWorldTexture(GLuint texture,  Entity *e,Camera *r, GLuint shader) {
    glUseProgram(shader);
    CheckGLError("gluseprogram");

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(r->projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(r->view));

    glBindVertexArray(e->VAO);
    glActiveTexture(GL_TEXTURE0);
    CheckGLError("glactivetexture");

    glBindTexture(GL_TEXTURE_2D, texture);
    CheckGLError("glBindTexture");
    glUniform1i(glGetUniformLocation(shader, "screenTexture"), 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

