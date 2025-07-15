#ifndef CAMERA_H 
#define CAMERA_H

#include "defines.h"

struct Camera {
    vec3 position;
    vec3 target;
    vec3 direction;
    vec3 right;
    vec3 up;

    mat4 view;

    bool debug = false;
};

inline Camera createCamera()
{
    Camera cam{};

    cam.position = vec3(0.0f, 0.0f, 3.0f);
    cam.target   = vec3(0.0f, 0.0f, 0.0f);

    const vec3 worldUp   = vec3(0.0f, 1.0f, 0.0f);           // must be non-zero
    cam.direction        = (cam.position - cam.target).normalize();
    cam.right            = (cross(worldUp, cam.direction)).normalize();
    cam.up               = cross(cam.direction, cam.right);  // already unit-length

    cam.view = mat4::lookAt(cam.position, cam.target, worldUp);

    return cam;  // returned by value (NRVO/RVO will elide copies)
}

#endif
