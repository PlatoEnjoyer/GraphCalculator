#include <ui_manager.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_glfw.h>
#include <iostream>



void preparing() {
    // Подготовка ImGui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void field_drawing(std::string& current_expr,  bool& expr_needs_update, char function_input[256], std::string& error_message, int i) {
    // Отрисовка ImGui окон
    std::string input_id = "##Input_" + std::to_string(i);
    ImGui::Text("%d f(x) = ", i);
    ImGui::SetNextWindowSize(ImVec2(100, 100));
    ImGui::SetNextWindowSize(ImVec2(100, 100), ImGuiCond_Always);
    ImGui::SameLine();
    if (ImGui::InputText("", function_input, 256)) {
        current_expr = std::string(function_input);
        expr_needs_update = true;
    }
    if (!error_message.empty())
        ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", error_message.c_str());
}

void mouse_wheel(float& zoom) {
    if (ImGui::GetIO().MouseWheel != 0.0f) {
        float zoom_factor = 1.1f;
        if (ImGui::GetIO().MouseWheel > 0) {
            zoom *= zoom_factor; // Увеличение
        } else {
            zoom /= zoom_factor; // Уменьшение
        }
    }
}