#include "camera.h"
#include "defines.h"

void Camera::init() {
    position = vec3(0.0f, 0.0f, 3.0f);
    target = vec3(0.0f, 0.0f, 0.0f);
    direction = (position - target).normalize();
    up = vec3(0.0f, 0.0f, 0.0f);
    cameraRight = cross(up, direction).normalize();
    cameraUp = cross(direction, cameraRight);
    //TODO
    view = mat4.lookAt();
}
