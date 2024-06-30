#include "camera.h"

void 
initializeCamera(camera *Camera)
{
    Camera->position =glm::vec3(-0.1f, 4.0f, 7.0f); 
    Camera->cameraFront = glm::vec3(-0.02f, -0.3f, -1.0f);
    Camera->up = glm::vec3(0.0f, 1.0f, 0.0f);

    Camera->yaw = -90.0f;
    Camera->pitch = 0.0f;
    Camera->fov = 45.0f;
    //winwidth and height need to set
    Camera->aspect_ratio = 1000.0f / 1000.0f;
    Camera->znear = 0.1f;
    Camera->zfar = 1000.0f;
    Camera -> speed = 100.0f;
}
