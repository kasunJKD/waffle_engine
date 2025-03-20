#include "font.h"
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

    //init Text rendering
    FontManager* fontManager = FontManager::GetInstance();
    fontManager->InitTextRendering();
    
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

void RenderText_f1(GLuint shader, std::string text, Entity *e, Camera *r)
{

    FontManager* fontManager = FontManager::GetInstance();
    glUseProgram(shader);

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(r->projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(r->view));
    glUniform3fv(glGetUniformLocation(shader, "worldPosition"), 1, glm::value_ptr(e->position));
    
GLint textColorLoc = glGetUniformLocation(shader, "textColor");
if (textColorLoc != -1) {
    glUniform3f(textColorLoc, e->color.x, e->color.y, e->color.z);
}

    GLint textAtlasLoc = glGetUniformLocation(shader, "textAtlas");
    if (textAtlasLoc != -1) {
        glUniform1i(textAtlasLoc, 0); // Texture unit 0
    }

    // ✅ Bind font texture to texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontManager->fontTexture_1);
    glBindVertexArray(fontManager->VAO);

    std::vector<float> vertices;
 glm::vec3 textPos = e->position;

    for (char c : text) {
        Character ch = fontManager->Characters_font1[c];

        float xpos = textPos.x + ch.bearing.x * e->scale;
        float ypos = textPos.y - (ch.size.y - ch.bearing.y) * e->scale;

        float w = ch.size.x * e->scale;
        float h = ch.size.y * e->scale;

        float u0 = ch.uv.x;
        float u1 = u0 + (ch.size.x / fontManager->atlas_width);

float quadVertices[6][4] = {
    { xpos,     ypos + h,   u0, 1.0f }, // Flip here (1.0 instead of 0.0)
    { xpos,     ypos,       u0, 0.0f }, // Flip here (0.0 instead of 1.0)
    { xpos + w, ypos,       u1, 0.0f },

    { xpos,     ypos + h,   u0, 1.0f },
    { xpos + w, ypos,       u1, 0.0f },
    { xpos + w, ypos + h,   u1, 1.0f }
};

        vertices.insert(vertices.end(), &quadVertices[0][0], &quadVertices[6][0]);

        textPos.x += (ch.advance * e->scale);
    }

    glBindBuffer(GL_ARRAY_BUFFER, fontManager->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
    //glDrawArrays(GL_TRIANGLES, 0, vertices.size() / 4);
    
    GLsizei vertexCount = vertices.size() / 4;
    if (vertexCount > 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDrawArrays(GL_TRIANGLES, 0, vertexCount);
        //glDisable(GL_BLEND);
    } else {
        printf("Error: No vertices to draw!\n");
    }
    
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

