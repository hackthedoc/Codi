#include "ImGuiLayer.h"

#include "Codi/Application.h"

#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"

namespace Codi {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::onUpdate() {

    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());
    
    float time = (float)glfwGetTime();
    io.DeltaTime = _time > 0.0f ? (time - _time) : (1.0f / 60.0f);
    _time = time;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    // Demo window (for testing)
    static bool show = true;
    ImGui::ShowDemoWindow(&show);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::onEvent(Event& e) {

}

}