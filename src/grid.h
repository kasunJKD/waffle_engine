#ifndef GRID_H
#define GRID_H


#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
//GRID====================================
struct Grid {
    unsigned int width;
    unsigned int depth;
    shader* gridshade;
    std::vector<float> gridVertices;
    GLuint VAO, VBO;

    Grid(unsigned int w, unsigned int d, shader* s)
    {
        this->width = w;
        this->depth = d;
        this->gridshade = s;
    }

    void Init()
    {
        // Clear previous data
        gridVertices.clear();

        float halfWidth = static_cast<float>(width) / 2.0f;
        float halfDepth = static_cast<float>(depth) / 2.0f;

        // Generate vertices for vertical lines
        for (unsigned int i = 0; i <= width; ++i) {
            float x = static_cast<float>(i) - halfWidth;
            // Vertical line from (x, 0, 0) to (x, 0, depth)
            gridVertices.push_back(x);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(-halfDepth);
            gridVertices.push_back(x);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(halfDepth);
        }

        // Generate vertices for horizontal lines
        for (unsigned int j = 0; j <= depth; ++j) {
            float z = static_cast<float>(j) - halfDepth;
            // Horizontal line from (0, 0, z) to (width, 0, z)
            gridVertices.push_back(-halfWidth);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(z);
            gridVertices.push_back(halfWidth);
            gridVertices.push_back(0.0f);
            gridVertices.push_back(z);
        }

        // Setup buffers
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
   
    void draw(const glm::mat4& view, const glm::mat4& projection)
    {
        // Bind VAO and draw the grid
        // Use shader program
        gridshade->bind();
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 MVP = projection * view * model;
        glUniformMatrix4fv(glGetUniformLocation(gridshade->shaderProgramId, "MVP"), 1, GL_FALSE, glm::value_ptr(MVP));

        glBindVertexArray(VAO);
        glDrawArrays(GL_LINES, 0, gridVertices.size() / 3);
        glBindVertexArray(0);   
    }

     ~Grid()
        {
            // Cleanup OpenGL resources
            glDeleteVertexArrays(1, &VAO);
            glDeleteBuffers(1, &VBO);
        }
};
#endif GRID_H
