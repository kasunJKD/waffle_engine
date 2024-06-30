#include <glm/glm.hpp>
struct camera
{
    glm::vec3 position;
    glm::vec3 cameraFront;
    glm::vec3 up;

    float yaw;
    float pitch;
    float fov;
    float aspect_ratio;
    float znear;
    float zfar;

    float speed;
};

void initializeCamera(camera *Camera);
