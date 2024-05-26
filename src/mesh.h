#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "shader.h"

#include "textureManager.h"

using namespace std;

struct Vertex
{ 
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

struct Texture
{
    unsigned int id;
    string type;
    string path;
};

struct Mesh
{
public:
    vector<Vertex>vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    Mesh(
    vector<Vertex>vertices,
    vector<GLuint> indices,
    vector<Texture> textures);

    void Draw(shader shader);

private:
	GLuint VAO, VBO, EBO;
	void setupMesh();


};
