#include "mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
TextureManager* TextureManager::m_inst = nullptr;
 
Mesh::Mesh(
    vector<Vertex>vertices,
    vector<GLuint> indices,
    vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    this->setupMesh();
}

 
void Mesh::Draw(shader shader)
{
    GLuint diffuseNr = 1, specularNr = 1;

    for (GLuint i = 0; i < this->textures.size(); i++)
    {
            glActiveTexture(GL_TEXTURE0 + i);

            stringstream ss;
            string number;
            string name = this->textures[i].type;
            if (name == "texture_diffuse") 
                    ss << diffuseNr++;
            else if (name == "texture_specular")
                    ss << specularNr++;
            number = ss.str();

            glUniform1i(glGetUniformLocation(shader.shaderProgramId, (name + number).c_str()), i);
            TextureManager::Inst()->BindTexture(this->textures[i].id);
    }

    glUniform1f(glGetUniformLocation(shader.shaderProgramId, "material.shininess"), 16.0f);

    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);

    for (GLuint i = 0; i < this->textures.size(); i++)
    {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);

    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex), &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint), &this->indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
