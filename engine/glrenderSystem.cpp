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

    //init Sprite rendering TODO
// Init Sprite Rendering Quad
    float quadVertices[] = {
        // positions    // texCoords
        0.0f, 1.0f,     0.0f, 1.0f, // top-left
        1.0f, 1.0f,     1.0f, 1.0f, // top-right
        1.0f, 0.0f,     1.0f, 0.0f, // bottom-right
        0.0f, 0.0f,     0.0f, 0.0f  // bottom-left
    };

    unsigned int quadIndices[] = {
        0, 1, 2,
        2, 3, 0
    };

    glGenVertexArrays(1, &rs->spriteVAO);
    glGenBuffers(1, &rs->spriteVBO);
    glGenBuffers(1, &rs->spriteEBO);

    glBindVertexArray(rs->spriteVAO);

    glBindBuffer(GL_ARRAY_BUFFER, rs->spriteVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rs->spriteEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    
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

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontManager->fontTexture_1);
    glBindVertexArray(fontManager->VAO);

    std::vector<float> vertices;

float cursorX = e->position.x;

for (char c : text) {
    Character ch = fontManager->Characters_font1[c];

    float xpos = cursorX + ch.bearing.x * e->scale;
    //float ypos = baselineY - (ch.bearing.y + ch.size.y) * e->scale;
float ypos = e->position.y - (ch.bearing.y * e->scale);

    float w = ch.size.x * e->scale;
    float h = ch.size.y * e->scale;

   // float u0 = ch.uv.x;
    //float u0 = ch.uv.x + (1.0f / fontManager->atlas_width); 
    //float u1 = u0 + ((ch.size.x - 1) / fontManager->atlas_width); 
 //   float u1 = u0 + (ch.size.x / fontManager->atlas_width);
float uvPadding = 0.5f / fontManager->atlas_width; // Move UVs inward slightly
float u0 = ch.uv.x + uvPadding;
float u1 = u0 + ((ch.size.x - 1) / fontManager->atlas_width);

    float quadVertices[6][4] = {
        { xpos,     ypos + h,   u0, 1.0f },
        { xpos,     ypos,       u0, 0.0f },
        { xpos + w, ypos,       u1, 0.0f },

        { xpos,     ypos + h,   u0, 1.0f },
        { xpos + w, ypos,       u1, 0.0f },
        { xpos + w, ypos + h,   u1, 1.0f }
    };

    vertices.insert(vertices.end(), &quadVertices[0][0], &quadVertices[6][0]);

    cursorX += ch.advance * e->scale;
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


void renderSprite(GLuint shader, Entity* e, Camera* camera, RenderSystem* rs) {
    if (!e || !e->sprite) return;

    glUseProgram(shader);
    CheckGLError("glUseProgram");

    glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(camera->projection));
    glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(camera->view));

    int framesPerRow = e->sprite->sheet_size.x / e->sprite->frame_size.x;
    int actualFrame = e->sprite->start_frame;  // Use current_frame for animation
    int frameX = actualFrame % framesPerRow;
    int frameY = actualFrame / framesPerRow;

    glm::vec2 uvOffset = {
        (frameX * e->sprite->frame_size.x + e->sprite->pixel_offset.x) / e->sprite->sheet_size.x,
        (frameY * e->sprite->frame_size.y + e->sprite->pixel_offset.y) / e->sprite->sheet_size.y
    };

    glm::vec2 uvSize = {
        e->sprite->frame_size.x / e->sprite->sheet_size.x,
        e->sprite->frame_size.y / e->sprite->sheet_size.y
    };

    glUniform2f(glGetUniformLocation(shader, "u_uv"), uvOffset.x, uvOffset.y);
    glUniform2f(glGetUniformLocation(shader, "u_uv_size"), uvSize.x, uvSize.y);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), e->position);
    model = glm::scale(model, glm::vec3(e->sprite->frame_size * e->scale, 1.0f));
    glUniformMatrix4fv(glGetUniformLocation(shader, "u_model"), 1, GL_FALSE, glm::value_ptr(model));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, e->sprite->texture_id);
    glUniform1i(glGetUniformLocation(shader, "u_tex"), 0);

    glBindVertexArray(rs->spriteVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
