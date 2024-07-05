#include <glm/glm.hpp>
struct camera
{
    glm::vec3 position;
    glm::vec3 cameraFront;
    glm::vec3 up;

    glm::vec3 old_position;

    float yaw;
    float pitch;
    float fov;
    float aspect_ratio;
    float znear;
    float zfar;

    float speed;
};



enum CameraPosition
{
    DEFAULT,
    TESTGROUND
};

void initializeCamera(camera *Camera);
void setCameraToTestGround(camera *Camera);
void resetCameraPos(camera *Camera);
