#ifndef TEST_H
#define TEST_H

#include <cstdio>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <stb_image.h>
#include "shader.h"

#include <iostream>
#include <string>
#include <vector>

struct TESTBED_cube
{
    std::vector<float> testCubeVertices;
    std::string file_path;
    glm::vec3 position;
    std::vector<glm::vec3> cube_positions;
};

struct TESTBED
{
    TESTBED_cube test_cube;
    shader* testShader;
    unsigned int VAO, VBO, planeVAO, planeVBO;
    unsigned int textureid, planetextureid;
    unsigned char* stbi_data;
    std::vector<float> planeVertices;
    std::string plane_texture_file_path;
    glm::vec3 plane_position;
};

TESTBED* initTestBed()
{
    // Allocate memory for the TESTBED structure
    TESTBED* testbed = new TESTBED();

    testbed->planeVertices = {
    // positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

         5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
         5.0f, -0.5f, -5.0f,  2.0f, 2.0f
    };

    testbed->plane_texture_file_path = "assets/metal.png";

    TESTBED_cube testcube = {};
    testcube.file_path = "assets/marble.jpg";
    testcube.testCubeVertices = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    testbed->test_cube = testcube;
    // Initialize other members of the TESTBED structure
    testbed->testShader = nullptr;
    testbed->VAO = 0;
    testbed->VBO = 0;
    testbed->textureid = 0;
    testbed->stbi_data = nullptr;
    testbed->test_cube.position = glm::vec3(-0.1f, 4.0f, 700.0f);
    testbed->test_cube.cube_positions = {
        glm::vec3( -0.1f,  1.0f, 700.0f),
        glm::vec3( 1.0f,  2.0f, 700.0f),
    };
    testbed->plane_position = glm::vec3(-0.1f, 1.0f, 700.0f);


    return testbed;
}

void initTestData(TESTBED* testbed)
{
    glGenVertexArrays(1, &testbed->VAO);
    glGenBuffers(1, &testbed->VBO);

    glBindVertexArray(testbed->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, testbed->VBO);
    glBufferData(GL_ARRAY_BUFFER, testbed->test_cube.testCubeVertices.size() * sizeof(float), testbed->test_cube.testCubeVertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //plane
    glGenVertexArrays(1, &testbed->planeVAO);
    glGenBuffers(1, &testbed->planeVBO);

    glBindVertexArray(testbed->planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, testbed->planeVBO);
    glBufferData(GL_ARRAY_BUFFER, testbed->planeVertices.size() * sizeof(float), testbed->planeVertices.data(), GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture 1
    // ---------
    glGenTextures(1, &testbed->textureid);
    glBindTexture(GL_TEXTURE_2D, testbed->textureid);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load(testbed->test_cube.file_path.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // testbed->testShader->bind();
    // glUniform1i(glGetUniformLocation(testbed->testShader->shaderProgramId, "texture1"), 0);
    // texture 2
    // ---------
    glGenTextures(1, &testbed->planetextureid);
    glBindTexture(GL_TEXTURE_2D, testbed->planetextureid);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    int width2, height2, nrChannels2;
    unsigned char *data2 = stbi_load(testbed->plane_texture_file_path.c_str(), &width2, &height2, &nrChannels2, 0);
    if (data2)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data2);

    testbed->testShader->bind();
    glUniform1i(glGetUniformLocation(testbed->testShader->shaderProgramId, "texture1"), 0);
}

void updateTestBed(TESTBED* testbed, glm::mat4 proj, glm::mat4 view)
{
    glm::mat4 model = glm::mat4(1.0f);
    
    // Bind texture on corresponding texture unit
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, testbed->textureid);

    // Activate shader
    testbed->testShader->bind();
    testbed->testShader->setMat4("projection", proj);
    testbed->testShader->setMat4("view", view);

    // Render the main cube
    model = glm::translate(model, testbed->test_cube.position);
    testbed->testShader->setMat4("model", model);

    glBindVertexArray(testbed->VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    for (const auto& cube_position : testbed->test_cube.cube_positions)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, cube_position);
        testbed->testShader->setMat4("model", model);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glBindVertexArray(testbed->planeVAO);
    glBindTexture(GL_TEXTURE_2D, testbed->planetextureid);
    model = glm::mat4(1.0f);
    model = glm::translate(model, testbed->plane_position);
    testbed->testShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void clearTest(TESTBED* test)
{
    glDeleteVertexArrays(1, &test->VAO);
    glDeleteBuffers(1, &test->VBO);
    delete test->testShader; // Delete the shader
    delete test;
}

#endif
