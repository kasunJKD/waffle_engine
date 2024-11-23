#pragma once

#include "entityManager.h"
#include "sceneManager.h"

#include <glad/glad.h>

struct RenderManager
{
    GLuint basicShader;
    GLuint VAO, VBO, EBO;

    void init();
    void update(const EntityManager& entityManager, const SceneManager& sceneManager);
    void cleanUp();
};

void quad_init(GLuint &VBO, GLuint &VAO, GLuint &EBO);
void quad_render(const TransformComponent& transform, const Shape_RectComponent& shape, GLuint shaderProgram);
