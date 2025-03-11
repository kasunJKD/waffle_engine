#ifndef GLRENDERSYSTEM_H
#define GLRENDERSYSTEM_H

#include "entity.h"
#include "resourceManager.h"
#include <string>
#define MAX_RENDER_COMPONENTS 128

struct RenderComponent {
    entityId entity_id;
    std::string  shaderName;      // Name of shader in ResourceManager
    std::string  textureName;     // Name of texture in ResourceManager
    GLuint VAO;             // The VAO handle (already set up with VBO/IBO)
    int    indexCount;      // How many indices to draw (if using glDrawElements)
    bool   useIndices;      // Whether we call glDrawElements or glDrawArrays
};

struct RenderSystem {
    RenderComponent components[MAX_RENDER_COMPONENTS];
    int count;

    ResourceManager* resManager;
};


void CheckGLError(const std::string& message);

void initRenderSystem(RenderSystem* rs, ResourceManager* rm);

RenderComponent* createRenderComponent(RenderSystem* rs, 
                     Entity* e,
                     std::string shaderName, 
                     std::string textureName,
                     GLuint VAO,
                     int indexCount,
                     bool useIndices);

void InitWorldQuad(RenderComponent* rc);

void RenderWorldTexture(GLuint texture,  Entity *e,RenderComponent *r, GLuint shader);

#endif //GLRENDERSYSTEM_H
