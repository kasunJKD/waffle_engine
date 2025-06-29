#pragma once

#include "defines.h"

struct Camera {
    vec3 position;
    vec3 target;
    vec3 direction;
    vec3 up;
    vec3 cameraRight;
    vec3 cameraUp;

    //TODO
    mat4 view;

    void init();
    void update();
    void deinit();
};
