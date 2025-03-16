#ifndef GLRENDERSYSTEM_H
#define GLRENDERSYSTEM_H

#pragma once
#include "resourceManager.h"
#include <string>
#include "entity.h"

void CheckGLError(const std::string& message);

void initRenderSystem(RenderSystem* rs, ResourceManager* rm);

void createRenderComponent(RenderSystem* rs, Entity* e);

void InitWorldQuad(Entity* rc);

void RenderWorldTexture(GLuint texture,  Entity *e,Camera *r, GLuint shader);

#endif //GLRENDERSYSTEM_H
