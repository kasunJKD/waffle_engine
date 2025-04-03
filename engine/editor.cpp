#include "editor.h"
#include "debug.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

void Editor::update_camera(Camera *camera)
{
    
    camera->projection = glm::ortho(
        0.0f, camera->width,    // left, right
        camera->height, 0.0f,   // bottom, top  (note the flip if you like Y up vs. Y down)
        -1.0f, 1.0f
    );

    camera->view = glm::translate(glm::mat4(1.0f), 
                                 glm::vec3(-camera->position.x, -camera->position.y, camera->position.z));
}

void Editor::activate_editor() {
    DEBUG_LOG("editor activated");
    this->active = !this->active;
}

void Editor::init_editor() {
    Entity* cam = create_entity("camera_editor");
    cam->type = CAMERA;
    Camera camera;
    camera.id = cam->id;
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.width = 512.0f;
    camera.height = 288.0f;

    this->camera = camera;
}

// void Editor::update_editor(InputManager *i) {
//     if(i->isMouseButtonScrollFlip((Uint8)SDL_MOUSEWHEEL)) {
//         int scrollY = i->getMouseWheelY();
//
//         if (scrollY != 0) {
//             this->camera.position.z -= scrollY * 0.1f;  // scroll up -> zoom in, scroll down -> zoom out
//             DEBUG_LOG("editor z pos %f\n", this->camera.position.z);
//         }
//     }
//
// }
void Editor::update_editor(InputManager* i, float dt) {
    float zoomSpeed = 50.0f;
    int scrollY = i->getMouseWheelY();
    float moveSpeed = 100.0f;

    if(i->isMouseButtonScrollFlip((Uint8)SDL_MOUSEWHEEL)) {
        if (scrollY != 0) {
            // Zoom in (scroll up): reduce width/height
            // Zoom out (scroll down): increase width/height
            camera.width += scrollY * zoomSpeed;
            camera.height += scrollY * (zoomSpeed * (camera.height / camera.width));

            // Optional: clamp zoom
            camera.width = std::max(64.0f, std::min(3072.0f, camera.width));
            camera.height = std::max(36.0f, std::min(1728.0f, camera.height));

            update_camera(&camera);

            DEBUG_LOG("Zoom: width=%.2f, height=%.2f\n", camera.width, camera.height);
        }
    }

    else if(i->isMouseWheelScroll((Uint8)SDL_MOUSEWHEEL)) {
        if (scrollY != 0) {
            // Zoom in (scroll up): reduce width/height
            // Zoom out (scroll down): increase width/height
            camera.width -= scrollY * zoomSpeed;
            camera.height -= scrollY * (zoomSpeed * (camera.height / camera.width));

            // Optional: clamp zoom
            camera.width = std::max(64.0f, std::min(3072.0f, camera.width));
            camera.height = std::max(36.0f, std::min(1728.0f, camera.height));

            update_camera(&camera);

            DEBUG_LOG("Zoom: width=%.2f, height=%.2f\n", camera.width, camera.height);
        }
    }

    if(i->isKeyPressed(SDLK_d) || i->isKeyHeld(SDLK_d)) {
        camera.position.x += moveSpeed * dt;
    }
    if(i->isKeyPressed(SDLK_a) || i->isKeyHeld(SDLK_a)) {
        camera.position.x -= moveSpeed * dt;
    }
    if(i->isKeyPressed(SDLK_w) || i->isKeyHeld(SDLK_w)) {
        camera.position.y -= moveSpeed * dt;
    }
    if(i->isKeyPressed(SDLK_s) || i->isKeyHeld(SDLK_s)) {
        camera.position.y += moveSpeed * dt;
    }

    update_camera(&camera);
}
