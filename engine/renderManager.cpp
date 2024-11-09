#include "renderManager.h"
#include "shader.h"

#include <glm/gtc/matrix_transform.hpp> // For glm::translate, glm::rotate, glm::scale
#include <glm/gtc/type_ptr.hpp>

void RenderManager::init()
{
    shader basic_shader_ = shader("engine/shaders/basic.vert", "engine/shaders/basic.frag");
    basicShader = basic_shader_.shaderProgramId;

    quad_init(VAO, VBO, EBO);
}

void RenderManager::update(const EntityManager& entityManager) {
    glUseProgram(basicShader);
    glBindVertexArray(VAO);

    for (const entity_id entity : entityManager.entities) { // Use entity_id directly
        if (entityManager.metadata.at(entity).active == false) continue;  // Skip inactive entities

        // Query the components present for this entity
        uint32_t componentMask = entityManager.queryComponents(entity);

        // Check if the entity has both Transform and Shape_Rect components
        if ((componentMask & ComponentType::Transform) && (componentMask & ComponentType::Shape_Rect)) {
            // Fetch components
            const TransformComponent& transform = entityManager.transforms.at(entity);
            const Shape_RectComponent& shapeRect = entityManager.shape_rects.at(entity);

            // Render the entity's quad
            quad_render(transform, shapeRect, basicShader);
        }
    }

    glBindVertexArray(0);
    glUseProgram(0);
}

void RenderManager::cleanUp()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(basicShader);
}


void quad_init(GLuint &VBO,GLuint &VAO,GLuint &EBO)
{
     const float RECTANGLE_VERTICES[] = {
        -1.0f, -1.0f,  // Bottom-left
         1.0f, -1.0f,  // Bottom-right
         1.0f,  1.0f,  // Top-right
        -1.0f,  1.0f   // Top-left
    };

    const unsigned int RECTANGLE_INDICES[] = {
        0, 1, 2,
        2, 3, 0
    };

    // Generate and bind VAO, VBO, and EBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(RECTANGLE_VERTICES), RECTANGLE_VERTICES, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(RECTANGLE_INDICES), RECTANGLE_INDICES, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void quad_render(const TransformComponent& transform, const Shape_RectComponent& shape, GLuint shaderProgram) {
    // Set color uniform
    GLint colorLoc = glGetUniformLocation(shaderProgram, "shapeColor");
    glUniform4f(colorLoc, shape.color.r, shape.color.g, shape.color.b, shape.color.a);

    // Calculate model matrix for the quad
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(transform.position, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotation), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(transform.scale * shape.size, 1.0f));

    // Set model matrix uniform
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, -1.0f, 1.0f); // Assuming 800x600 window
GLint projectionLoc = glGetUniformLocation(shaderProgram, "projection");
glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

    // Render the quad
    if (shape.filled) {
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_INT, 0);
    }
}