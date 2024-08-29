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
struct TESTBED_grass 
{
    std::vector<float> grassVertices;
    std::string file_path;
    std::vector<glm::vec3> grass_positions;
};

struct TESTBED
{
    TESTBED_cube test_cube;
    TESTBED_grass test_grass;
    shader* testShader;
    shader* grassShader;
    unsigned int VAO, VBO, planeVAO, planeVBO, grassVAO, grassVBO;
    unsigned int textureid, planetextureid, grassTextureId;
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
    testbed->grassShader = nullptr;
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
    
    TESTBED_grass test_grass = {};
    test_grass.grassVertices = {
        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

        0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
        1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
        1.0f,  0.5f,  0.0f,  1.0f,  0.0f
    };
    test_grass.file_path = "assets/grass.png";
    test_grass.grass_positions = {
        glm::vec3(-1.5f, 1.0f, 700.48f),
        glm::vec3( 1.5f, 1.0f, 700.51f),
        glm::vec3( 0.0f, 1.0f, 700.7f),
        glm::vec3(-0.3f, 1.0f, 700.3f),
        glm::vec3 (0.5f, 1.0f, 700.6f)
    };
    testbed->test_grass = test_grass;

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
    //grass
    glGenVertexArrays(1, &testbed->grassVAO);
    glGenBuffers(1, &testbed->grassVBO);

    glBindVertexArray(testbed->grassVAO);

    glBindBuffer(GL_ARRAY_BUFFER, testbed->grassVAO);
    glBufferData(GL_ARRAY_BUFFER, testbed->test_grass.grassVertices.size() * sizeof(float), testbed->test_grass.grassVertices.data(), GL_STATIC_DRAW);

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
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
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
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
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

    glGenTextures(1, &testbed->grassTextureId);
    glBindTexture(GL_TEXTURE_2D, testbed->grassTextureId);
    // load image, create texture and generate mipmaps
    //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    int width3, height3, nrChannels3;
    unsigned char *data3 = stbi_load(testbed->test_grass.file_path.c_str(), &width3, &height3, &nrChannels3, 0);
    if (data3)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, data3);
        glGenerateMipmap(GL_TEXTURE_2D);

    GLenum format;
        if (nrChannels3 == 1)
            format = GL_RED;
        else if (nrChannels3 == 3)
            format = GL_RGB;
        else if (nrChannels3 == 4)
            format = GL_RGBA;
    // set the texture wrapping parameters
     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data3);
    testbed->grassShader->bind();
    glUniform1i(glGetUniformLocation(testbed->grassShader->shaderProgramId, "texture1"), 0);
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
    
    //grass

    // Activate shader
    testbed->grassShader->bind();
    testbed->grassShader->setMat4("projection", proj);
    testbed->grassShader->setMat4("view", view);
    testbed->grassShader->setMat4("model", model);

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(testbed->grassVAO);
    glBindTexture(GL_TEXTURE_2D, testbed->grassTextureId);

     for (const auto& grass_position : testbed->test_grass.grass_positions)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, grass_position);
        testbed->grassShader->setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }


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
