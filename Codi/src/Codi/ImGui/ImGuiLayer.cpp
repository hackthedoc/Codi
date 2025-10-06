#include "cdpch.h"
#include "ImGuiLayer.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "Codi/Core/Application.h"

// TODO: remove temporary includes
#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Codi {

ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

ImGuiLayer::~ImGuiLayer() {}

void ImGuiLayer::onAttach() {
    CODI_PROFILE_FUNCTION();
    
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    
    io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Regular.ttf", 18.0f);
    io.Fonts->AddFontFromFileTTF("assets/fonts/opensans/OpenSans-Bold.ttf", 18.0f);

    setDarkThemeColors();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app = Application::Get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.getWindow().getNativeWindow());
    
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 410");
}

void ImGuiLayer::onDetach() {
    CODI_PROFILE_FUNCTION();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void ImGuiLayer::onEvent(Event& e) {
    if (!_blockEvents) return;

    ImGuiIO& io = ImGui::GetIO();
    if (e.isInCategory(EventCategoryMouse) & io.WantCaptureMouse ||
        e.isInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard
    ) e.handle();
}

void ImGuiLayer::begin() {
    CODI_PROFILE_FUNCTION();
    
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void ImGuiLayer::end() {
    CODI_PROFILE_FUNCTION();
    
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::Get();
    io.DisplaySize = ImVec2((float)app.getWindow().getWidth(), (float)app.getWindow().getHeight());
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backupCurrentContext);
    }
}

void ImGuiLayer::blockEvents(const bool flag) {
    _blockEvents = flag;
}

void ImGuiLayer::setDarkThemeColors() {
    ImGuiStyle& style = ImGui::GetStyle();

    const ImVec4 dracBackground         = ImVec4(0.14f, 0.14f, 0.18f, 1.0f);  // Background
    const ImVec4 dracDarkerBrackground  = ImVec4(0.10f, 0.10f, 0.13f, 1.0f);  // Darker background
    const ImVec4 dracLighterBackground  = ImVec4(0.18f, 0.18f, 0.22f, 1.0f);  // Lighter background
    const ImVec4 dracPurple             = ImVec4(0.55f, 0.27f, 0.68f, 1.0f);  // Accent purple
    const ImVec4 dracCyan               = ImVec4(0.50f, 0.86f, 0.88f, 1.0f);  // Cyan highlight
    const ImVec4 dracGreen              = ImVec4(0.49f, 0.84f, 0.65f, 1.0f);  // Green accent
    const ImVec4 dracOrange             = ImVec4(0.98f, 0.63f, 0.39f, 1.0f);  // Orange
    const ImVec4 dracPink               = ImVec4(1.00f, 0.52f, 0.76f, 1.0f);  // Pink
    const ImVec4 dracRed                = ImVec4(0.99f, 0.38f, 0.47f, 1.0f);  // Red
    const ImVec4 dracYellow             = ImVec4(0.99f, 0.84f, 0.51f, 1.0f);  // Yellow
    const ImVec4 dracWhite              = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);  // White text
    const ImVec4 dracGrey               = ImVec4(0.60f, 0.60f, 0.67f, 1.0f);  // Disabled text / subtle elements

    // Main colors
    style.Colors[ImGuiCol_Text]                   = dracWhite;
    style.Colors[ImGuiCol_TextDisabled]           = dracGrey;
    style.Colors[ImGuiCol_WindowBg]               = dracBackground;
    style.Colors[ImGuiCol_ChildBg]                = dracBackground;
    style.Colors[ImGuiCol_PopupBg]                = dracLighterBackground;
    style.Colors[ImGuiCol_Border]                 = dracLighterBackground;
    style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0, 0, 0, 0);

    // Frames
    style.Colors[ImGuiCol_FrameBg]                = dracDarkerBrackground;
    style.Colors[ImGuiCol_FrameBgHovered]         = dracLighterBackground;
    style.Colors[ImGuiCol_FrameBgActive]          = dracPurple;

    // Title bars
    style.Colors[ImGuiCol_TitleBg]                = dracDarkerBrackground;
    style.Colors[ImGuiCol_TitleBgActive]          = dracLighterBackground;
    style.Colors[ImGuiCol_TitleBgCollapsed]       = dracDarkerBrackground;

    // Menus / Tabs
    style.Colors[ImGuiCol_MenuBarBg]              = dracLighterBackground;
    style.Colors[ImGuiCol_Tab]                    = dracDarkerBrackground;
    style.Colors[ImGuiCol_TabHovered]             = dracPurple;
    style.Colors[ImGuiCol_TabActive]              = dracPurple;
    style.Colors[ImGuiCol_TabUnfocused]           = dracDarkerBrackground;
    style.Colors[ImGuiCol_TabUnfocusedActive]     = dracLighterBackground;

    // Scrollbars
    style.Colors[ImGuiCol_ScrollbarBg]            = dracBackground;
    style.Colors[ImGuiCol_ScrollbarGrab]          = dracLighterBackground;
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = dracPurple;
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = dracCyan;

    // Buttons
    style.Colors[ImGuiCol_Button]                 = dracLighterBackground;
    style.Colors[ImGuiCol_ButtonHovered]          = dracPurple;
    style.Colors[ImGuiCol_ButtonActive]           = dracPink;

    // Headers
    style.Colors[ImGuiCol_Header]                 = dracLighterBackground;
    style.Colors[ImGuiCol_HeaderHovered]          = dracPurple;
    style.Colors[ImGuiCol_HeaderActive]           = dracCyan;

    // Checkmarks, sliders, and selections
    style.Colors[ImGuiCol_CheckMark]              = dracCyan;
    style.Colors[ImGuiCol_SliderGrab]             = dracPurple;
    style.Colors[ImGuiCol_SliderGrabActive]       = dracPink;
    style.Colors[ImGuiCol_ResizeGrip]             = dracLighterBackground;
    style.Colors[ImGuiCol_ResizeGripHovered]      = dracPurple;
    style.Colors[ImGuiCol_ResizeGripActive]       = dracCyan;

    // Separators
    style.Colors[ImGuiCol_Separator]              = dracLighterBackground;
    style.Colors[ImGuiCol_SeparatorHovered]       = dracPurple;
    style.Colors[ImGuiCol_SeparatorActive]        = dracPink;

    // Docking
    style.Colors[ImGuiCol_DockingPreview]         = dracCyan;
    style.Colors[ImGuiCol_DockingEmptyBg]         = dracDarkerBrackground;

    // Misc
    style.Colors[ImGuiCol_PlotLines]              = dracCyan;
    style.Colors[ImGuiCol_PlotLinesHovered]       = dracPink;
    style.Colors[ImGuiCol_PlotHistogram]          = dracOrange;
    style.Colors[ImGuiCol_PlotHistogramHovered]   = dracYellow;
    style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(dracPurple.x, dracPurple.y, dracPurple.z, 0.35f);

    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_NavHighlight]           = dracCyan;
    style.Colors[ImGuiCol_NavWindowingHighlight]  = dracCyan;
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_DragDropTarget]         = dracPink;

    // Style Adjustments
    style.FrameRounding      = 5.0f;
    style.GrabRounding       = 4.0f;
    style.TabRounding        = 4.0f;
    style.WindowRounding     = 6.0f;
    style.ScrollbarRounding  = 6.0f;

    style.WindowPadding      = ImVec2(8.0f, 8.0f);
    style.FramePadding       = ImVec2(6.0f, 4.0f);
    style.ItemSpacing        = ImVec2(6.0f, 6.0f);
    style.IndentSpacing      = 20.0f;
    style.ScrollbarSize      = 16.0f;
}

void ImGuiLayer::setLightThemeColors() {
    ImGuiStyle& style = ImGui::GetStyle();
    
    const ImVec4 background     = ImVec4(0.95f, 0.95f, 0.95f, 1.0f);  // Window background
    const ImVec4 backgroundAlt  = ImVec4(0.90f, 0.90f, 0.90f, 1.0f);  // Alternate background (frames, headers)
    const ImVec4 border         = ImVec4(0.80f, 0.80f, 0.80f, 1.0f);  // Borders
    const ImVec4 text           = ImVec4(0.10f, 0.10f, 0.10f, 1.0f);  // Main text
    const ImVec4 textDisabled   = ImVec4(0.45f, 0.45f, 0.45f, 1.0f); // Disabled text
    const ImVec4 primary        = ImVec4(0.30f, 0.60f, 0.90f, 1.0f);  // Buttons, highlights
    const ImVec4 hover          = ImVec4(0.40f, 0.70f, 1.0f, 1.0f);   // Hovered buttons / tabs
    const ImVec4 active         = ImVec4(0.25f, 0.55f, 0.85f, 1.0f);  // Active / pressed
    const ImVec4 accent         = ImVec4(0.95f, 0.58f, 0.18f, 1.0f);  // Accent color (sliders, highlights)
    const ImVec4 selected       = ImVec4(0.30f, 0.60f, 0.90f, 0.35f); // Selection background

    // Main colors
    style.Colors[ImGuiCol_Text]                   = text;
    style.Colors[ImGuiCol_TextDisabled]           = textDisabled;
    style.Colors[ImGuiCol_WindowBg]               = background;
    style.Colors[ImGuiCol_ChildBg]                = background;
    style.Colors[ImGuiCol_PopupBg]                = backgroundAlt;
    style.Colors[ImGuiCol_Border]                 = border;
    style.Colors[ImGuiCol_BorderShadow]           = ImVec4(0, 0, 0, 0);

    // Frames
    style.Colors[ImGuiCol_FrameBg]                = backgroundAlt;
    style.Colors[ImGuiCol_FrameBgHovered]         = hover;
    style.Colors[ImGuiCol_FrameBgActive]          = active;

    // Title bars
    style.Colors[ImGuiCol_TitleBg]                = backgroundAlt;
    style.Colors[ImGuiCol_TitleBgActive]          = backgroundAlt;
    style.Colors[ImGuiCol_TitleBgCollapsed]       = background;

    // Menus / Tabs
    style.Colors[ImGuiCol_MenuBarBg]              = backgroundAlt;
    style.Colors[ImGuiCol_Tab]                    = backgroundAlt;
    style.Colors[ImGuiCol_TabHovered]             = hover;
    style.Colors[ImGuiCol_TabActive]              = active;
    style.Colors[ImGuiCol_TabUnfocused]           = backgroundAlt;
    style.Colors[ImGuiCol_TabUnfocusedActive]     = backgroundAlt;

    // Scrollbars
    style.Colors[ImGuiCol_ScrollbarBg]            = background;
    style.Colors[ImGuiCol_ScrollbarGrab]          = hover;
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = active;
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = primary;

    // Buttons
    style.Colors[ImGuiCol_Button]                 = primary;
    style.Colors[ImGuiCol_ButtonHovered]          = hover;
    style.Colors[ImGuiCol_ButtonActive]           = active;

    // Headers
    style.Colors[ImGuiCol_Header]                 = backgroundAlt;
    style.Colors[ImGuiCol_HeaderHovered]          = hover;
    style.Colors[ImGuiCol_HeaderActive]           = active;

    // Checkmarks, sliders, and selections
    style.Colors[ImGuiCol_CheckMark]              = accent;
    style.Colors[ImGuiCol_SliderGrab]             = primary;
    style.Colors[ImGuiCol_SliderGrabActive]       = hover;
    style.Colors[ImGuiCol_ResizeGrip]             = backgroundAlt;
    style.Colors[ImGuiCol_ResizeGripHovered]      = hover;
    style.Colors[ImGuiCol_ResizeGripActive]       = primary;

    // Separators
    style.Colors[ImGuiCol_Separator]              = border;
    style.Colors[ImGuiCol_SeparatorHovered]       = hover;
    style.Colors[ImGuiCol_SeparatorActive]        = active;

    // Docking
    style.Colors[ImGuiCol_DockingPreview]         = primary;
    style.Colors[ImGuiCol_DockingEmptyBg]         = background;

    // Misc
    style.Colors[ImGuiCol_PlotLines]              = primary;
    style.Colors[ImGuiCol_PlotLinesHovered]       = hover;
    style.Colors[ImGuiCol_PlotHistogram]          = accent;
    style.Colors[ImGuiCol_PlotHistogramHovered]   = primary;
    style.Colors[ImGuiCol_TextSelectedBg]         = selected;

    style.Colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    style.Colors[ImGuiCol_NavHighlight]           = primary;
    style.Colors[ImGuiCol_NavWindowingHighlight]  = primary;
    style.Colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.3f);
    style.Colors[ImGuiCol_DragDropTarget]         = hover;

    // Style Adjustments
    style.FrameRounding      = 5.0f;
    style.GrabRounding       = 4.0f;
    style.TabRounding        = 4.0f;
    style.WindowRounding     = 6.0f;
    style.ScrollbarRounding  = 6.0f;

    style.WindowPadding      = ImVec2(8.0f, 8.0f);
    style.FramePadding       = ImVec2(6.0f, 4.0f);
    style.ItemSpacing        = ImVec2(6.0f, 6.0f);
    style.IndentSpacing      = 20.0f;
    style.ScrollbarSize      = 16.0f;
}

}