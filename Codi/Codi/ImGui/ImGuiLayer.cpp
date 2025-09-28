#include "ImGuiLayer.h"

#include "imgui.h"

#define IMGUI_IMPL_API
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Codi/Application.h"

// TODO: remove temporary includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Codi {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui::StyleColorsDark();

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.ConfigFlags |= 
    
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::onUpdate() {
    Application& app = Application::Get();
    
    glViewport(0, 0, app.getWindow().getWidth(), app.getWindow().getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    ImGuiIO& io = ImGui::GetIO();
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
    EventDispatcher dispatcher(e);
    dispatcher.dispatch<MouseButtonPressedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onMouseButtonPressedEvent));
    dispatcher.dispatch<MouseButtonReleasedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onMouseButtonReleasedEvent));
    dispatcher.dispatch<MouseMovedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onMouseMovedEvent));
    dispatcher.dispatch<MouseScrolledEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onMouseScrolledEvent));
    dispatcher.dispatch<KeyPressedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onKeyPressedEvent));
    dispatcher.dispatch<KeyReleasedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onKeyReleasedEvent));
    dispatcher.dispatch<KeyTypedEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onKeyTypedEvent));
    dispatcher.dispatch<WindowResizeEvent>(CODI_BIND_EVENT_FN(ImGuiLayer::onWindowResizedEvent));
}

bool ImGuiLayer::onMouseButtonPressedEvent(MouseButtonPressedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = true;

    return false;
}

bool ImGuiLayer::onMouseButtonReleasedEvent(MouseButtonReleasedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[e.GetMouseButton()] = false;

    return false;
}

bool ImGuiLayer::onMouseMovedEvent(MouseMovedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(e.getX(), e.getY());

    return false;
}

bool ImGuiLayer::onMouseScrolledEvent(MouseScrolledEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += e.getXOffset();
    io.MouseWheel += e.getYOffset();
`
    return false;
}

ImGuiKey KeyCodeToImGuiKey(int key) {
    switch (key) {
        case GLFW_KEY_TAB: return ImGuiKey_Tab;
        case GLFW_KEY_LEFT: return ImGuiKey_LeftArrow;
        case GLFW_KEY_RIGHT: return ImGuiKey_RightArrow;
        case GLFW_KEY_UP: return ImGuiKey_UpArrow;
        case GLFW_KEY_DOWN: return ImGuiKey_DownArrow;
        case GLFW_KEY_PAGE_UP: return ImGuiKey_PageUp;
        case GLFW_KEY_PAGE_DOWN: return ImGuiKey_PageDown;
        case GLFW_KEY_HOME: return ImGuiKey_Home;
        case GLFW_KEY_END: return ImGuiKey_End;
        case GLFW_KEY_INSERT: return ImGuiKey_Insert;
        case GLFW_KEY_DELETE: return ImGuiKey_Delete;
        case GLFW_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case GLFW_KEY_SPACE: return ImGuiKey_Space;
        case GLFW_KEY_ENTER: return ImGuiKey_Enter;
        case GLFW_KEY_ESCAPE: return ImGuiKey_Escape;
        case GLFW_KEY_A: return ImGuiKey_A;
        case GLFW_KEY_C: return ImGuiKey_C;
        case GLFW_KEY_V: return ImGuiKey_V;
        case GLFW_KEY_X: return ImGuiKey_X;
        case GLFW_KEY_Y: return ImGuiKey_Y;
        case GLFW_KEY_Z: return ImGuiKey_Z;
        default: return ImGuiKey_None; // fallback for unsupported keys
    }
}

bool ImGuiLayer::onKeyPressedEvent(KeyPressedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();

    io.AddKeyEvent(KeyCodeToImGuiKey(e.getKeyCode()), true);

    return false;
}

bool ImGuiLayer::onKeyReleasedEvent(KeyReleasedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.AddKeyEvent(KeyCodeToImGuiKey(e.getKeyCode()), false);

    return false;
}

bool ImGuiLayer::onKeyTypedEvent(KeyTypedEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    int keycode = e.getKeyCode();
    if (keycode > 0 && keycode < 0x10000)
        io.AddInputCharacter((uint)keycode);
    
    return false;
}

bool ImGuiLayer::onWindowResizedEvent(WindowResizeEvent& e) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(e.getWidth(), e.getHeight());
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
    glViewport(0, 0, e.getWidth(), e.getHeight());

    return false;
}

}