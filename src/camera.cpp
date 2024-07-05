#include "camera.h"

// Static function declaration for internal use
static glm::vec3 getCameraPosition(CameraPosition pos);

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

void
setCameraToTestGround(camera *Camera)
{
    Camera->old_position = Camera->position;
    
    Camera->position = getCameraPosition(TESTGROUND);
}

static glm::vec3 getCameraPosition(CameraPosition pos) {
    switch (pos) {
        case DEFAULT:
            return glm::vec3(-0.1f, 4.0f, 7.0f);
        case TESTGROUND:
            return glm::vec3(-0.1f, 4.0f, 700.0f);
        default:
            return glm::vec3(0.0f, 0.0f, 0.0f); // Default case
    }
}

void
resetCameraPos(camera *Camera)
{
    Camera->position = Camera->old_position;
}
