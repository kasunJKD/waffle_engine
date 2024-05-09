#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>

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
};

struct Mesh
{
    
};
