#include "editor.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl2.h"

void Editor::update() {
    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
     // === Sprite Browser Panel ===
    //static Sprite* selectedSprite = nullptr;

    ImGui::Begin("Sprite Browser");

    // Fetch list once per frame

    // Show sprites in a scrolling area
    ImGui::BeginChild("##sprite_scroll", ImVec2(0,300), true);
    
    ImGui::EndChild();
    ImGui::End();

    bool value = true;
    if (value)
            ImGui::ShowDemoWindow(&value);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

void Editor::activate_editor(Editor* editor) {
    editor->active = true;
}
