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

void Editor::update_camera(Camera *cam)
{
  float aspect = cam->width / cam->height;
    cam->projection = glm::perspective(
        glm::radians(cam->fov),  // vertical FOV from camera
        aspect,
        0.1f,                    // near
        1000.0f                  // far
    );
    // preserve your “Y down” convention if desired:
    cam->projection[1][1] *= -1.0f;

    // 2) Recompute front vector from yaw/pitch
    glm::vec3 f;
    f.x = cos(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    f.y = sin(glm::radians(cam->pitch));
    f.z = sin(glm::radians(cam->yaw)) * cos(glm::radians(cam->pitch));
    cam->front = glm::normalize(f);

    // 3) View matrix: look from position toward position+front
    cam->view = glm::lookAt(
        cam->position,
        cam->position + cam->front,
        cam->up
    );

}

void Editor::activate_editor() {
    DEBUG_LOG("editor activated");
    this->active = !this->active;
}

void Editor::init_editor(SpriteManager *sp_mgr) {
    Camera camera;
    camera.type = CAMERA;
    camera.position = glm::vec3(0.0f, 0.0f, 0.0f);
    camera.position.x = 512  * 0.5f;
    camera.position.y = 288 * 0.5f;
    camera.width = 512.0f;
    camera.height = 288.0f;
    camera.front   = glm::vec3(0.0f, 0.0f, -1.0f);
    camera.up = glm::vec3(0.0f, 1.0f,  0.0f);
    camera.yaw     = -90.0f;
    camera.pitch   =   0.0f;
    camera.fov     =  60.0f;
    
    this->camera = camera;
    this->sp_mgr = sp_mgr;


}

void Editor::update_editor(InputManager* i, float dt) {
    // ─── tunables ─────────────────────────────────────────
    const float edgePanSpeed   = 200.0f;      // units/sec near screen edge
    const float dragPanSpeed   = 0.5f;        // world‐units per pixel drag
    const float zoomFactor     = 1.1f;        // 10% zoom per wheel notch
    const float edgeZonePx     = 20.0f;       // px inside window to start edge‐pan

    // ─── 0) Mouse‑delta / first‑mouse init ────────────────
    static bool firstMouse = true;
    static int  lastX = 0, lastY = 0;
    int mx = i->getMouseX();
    int my = i->getMouseY();
    if (firstMouse) {
        lastX = mx;
        lastY = my;
        firstMouse = false;
    }
    int dx = mx - lastX;
    int dy = lastY - my;  // invert: moving up → positive

    // ─── 1) LOOK‑AROUND (LMB) ──────────────────────────────
    if (i->isMouseButtonHeld(SDL_BUTTON_LEFT)) {
        camera.yaw   += dx * mouseSensitivity;
        camera.pitch += dy * mouseSensitivity;
        camera.pitch  = glm::clamp(camera.pitch, -89.0f, 89.0f);

        // rebuild front vector immediately
        glm::vec3 f;
        f.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        f.y = sin(glm::radians(camera.pitch));
        f.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
        camera.front = glm::normalize(f);
    }

    // ─── 2) WASD “fly” movement ──────────────────────────────
    float velocity = movementSpeed * dt;  // movementSpeed = your units/sec
    glm::vec3 forward = camera.front;
    glm::vec3 right   = glm::normalize(glm::cross(forward, camera.up));

    if (i->isKeyHeld(SDLK_w)) camera.position += forward * velocity;
    if (i->isKeyHeld(SDLK_s)) camera.position -= forward * velocity;
    if (i->isKeyHeld(SDLK_a)) camera.position -= right   * velocity;
    if (i->isKeyHeld(SDLK_d)) camera.position += right   * velocity;

    // ─── 3) ZOOM (wheel) ────────────────────────────────────
    int wheelY = i->getMouseWheelY();
    if (wheelY != 0) {
        float factor      = std::pow(zoomFactor, wheelY);
        camera.width    /= factor;
        camera.height   /= factor;
        camera.width     = glm::clamp(camera.width,  64.0f, 3072.0f);
        camera.height    = glm::clamp(camera.height, 36.0f, 1728.0f);
    }

    // ─── 4) EDGE‑PAN (mouse at screen border) ───────────────
    if (mx <  edgeZonePx)                     camera.position.x -= edgePanSpeed * dt;
    else if (mx > 960  - edgeZonePx) camera.position.x += edgePanSpeed * dt;
    if (my <  edgeZonePx)                     camera.position.y -= edgePanSpeed * dt;
    else if (my > 540 - edgeZonePx) camera.position.y += edgePanSpeed * dt;

    // ─── 5) DRAG‑PAN (MMB or RMB) ──────────────────────────
    if (i->isMouseButtonHeld(SDL_BUTTON_MIDDLE) ||
        i->isMouseButtonHeld(SDL_BUTTON_RIGHT)) {
        camera.position.x -= dx * dragPanSpeed;
        camera.position.y += dy * dragPanSpeed;
    }

    // ─── 6) Commit last mouse for next frame ───────────────
    lastX = mx;
    lastY = my;
    update_camera(&camera);

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
     // === Sprite Browser Panel ===
    //static Sprite* selectedSprite = nullptr;

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
            //selectedSprite = s;
        }
        ImGui::SameLine();
        ImGui::TextUnformatted(name.c_str());
        ImGui::PopID();
    }
    
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
