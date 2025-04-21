#include "editor.h"
#include "debug.h"
#include "engine.h"
#include "entity.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_transform.hpp"

#ifdef DEBUG_ENABLED
#include "imgui.h"
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_opengl3.h"
#endif

void Editor::update_camera(Camera *camera)
{
    
    camera->projection = glm::ortho(
        0.0f, camera->width,    // left, right
        camera->height, 0.0f,   // bottom, top  (note the flip if you like Y up vs. Y down)
        -10.0f, 10.0f
    );

    camera->view = glm::translate(glm::mat4(1.0f), 
                                 glm::vec3(-camera->position.x, -camera->position.y, camera->position.z));
}

void Editor::activate_editor() {
    DEBUG_LOG("editor activated");
    this->active = !this->active;
}

void Editor::init_editor(SpriteManager *sp_mgr) {
    Camera camera;
    camera.type = CAMERA;
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.width = 512.0f;
    camera.height = 288.0f;

    this->camera = camera;
    this->sp_mgr = sp_mgr;
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

            //DEBUG_LOG("Zoom: width=%.2f, height=%.2f\n", camera.width, camera.height);
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

            //DEBUG_LOG("Zoom: width=%.2f, height=%.2f\n", camera.width, camera.height);
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


    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
     // === Sprite Browser Panel ===
    static Sprite* selectedSprite = nullptr;

    ImGui::Begin("Sprite Browser");

    // Fetch list once per frame
    auto list = sp_mgr->getAllSprites();  

    // Show sprites in a scrolling area
    ImGui::BeginChild("##sprite_scroll", ImVec2(0,300), true);
    for (auto& [name, s] : list) {
        // compute UV coords in atlas
        float u0 = (s->pixel_offset.x)                                       / float(s->sheet_size.x);
        float v0 = (s->pixel_offset.y)                                       / float(s->sheet_size.y);
        float u1 = (s->pixel_offset.x + s->frame_size.x)                     / float(s->sheet_size.x);
        float v1 = (s->pixel_offset.y + s->frame_size.y)                     / float(s->sheet_size.y);

        ImGui::PushID(name.c_str());
        // use a small thumbnail size (e.g. 64×64)
        if (ImGui::ImageButton("b", (ImTextureID)s->texture_id, ImVec2(64,64),
                               ImVec2(u0,v0), ImVec2(u1,v1))) {
            selectedSprite = s;
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(name.c_str());
        ImGui::PopID();
    }
    DEBUG_LOG("TODO sprite editor",selectedSprite);
    
    ImGui::EndChild();
    ImGui::End();
}


void Editor::deinit_editor() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

void Editor::draw_editor() {

                ImGui::Render();
                ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
