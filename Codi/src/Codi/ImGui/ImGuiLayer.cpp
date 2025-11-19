#include "codipch.h"
#include "ImGuiLayer.h"

#include "Codi/Core/Application.h"
#include "Codi/Renderer/Renderer.h"

// TODO: make this platform abstract
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_vulkan.h>

namespace Codi {

    ImGuiLayer::ImGuiLayer() : Layer("ImGuiLayer") {}

    ImGuiLayer::~ImGuiLayer() {}

    void ImGuiLayer::OnAttach() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();

        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        //io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Regular.ttf", 18.0f);
        //io.Fonts->AddFontFromFileTTF("Resources/Fonts/OpenSans/OpenSans-Bold.ttf", 18.0f);

        ImGui::StyleColorsDark();

        Application& app = Application::Get();
        SDL_Window* window = static_cast<SDL_Window*>(app.GetWindow()->GetNativeWindow());
        ImGui_ImplSDL3_InitForVulkan(window);

        VulkanRendererAPI& vulkanAPI = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());

        ImGui_ImplVulkan_InitInfo info{};
        info.Instance = vulkanAPI.GetContext()->GetInstance();
        info.PhysicalDevice = vulkanAPI.GetContext()->GetPhysicalDevice();
        info.Device = vulkanAPI.GetContext()->GetLogicalDevice();
        info.QueueFamily = vulkanAPI.GetContext()->GetQueueIndices().GraphicsFamily;
        info.Queue = vulkanAPI.GetContext()->GetGraphicsQueue();
        info.PipelineCache = VK_NULL_HANDLE;
        info.DescriptorPool = vulkanAPI.GetImGuiDescriptorPool()->GetHandle();
        info.MinImageCount = vulkanAPI.GetSwapchain()->GetMaxFramesInFlight();
        info.ImageCount = vulkanAPI.GetSwapchain()->GetImageCount();
        info.Allocator = VulkanRendererAPI::GetAllocator();
        info.PipelineInfoMain.RenderPass = vulkanAPI.GetMainRenderPass()->GetHandle();
        info.PipelineInfoMain.Subpass = 0;
        info.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        info.CheckVkResultFn = [](VkResult err) { CODI_CORE_ASSERT(err == VK_SUCCESS); };

        ImGui_ImplVulkan_Init(&info);
    }

    void ImGuiLayer::OnDetach() {
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }

    void ImGuiLayer::OnEvent(Event& e) {
        // nothing yet
    }

    void ImGuiLayer::Begin() {
        if (Renderer::IsFrameSkipped()) return;

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();
        ImGui::ShowDemoWindow();
    }

    void ImGuiLayer::End() {
        if (Renderer::IsFrameSkipped()) return;

        ImGuiIO& io = ImGui::GetIO();
        Application& app = Application::Get();
        io.DisplaySize = ImVec2((float32)app.GetWindow()->GetWidth(), (float32)app.GetWindow()->GetHeight());

        ImGui::Render();
        VulkanRendererAPI& vulkanAPI = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkanAPI.GetCommandBuffer(vulkanAPI.GetCurrentFrameIndex())->GetHandle());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

} // namespace Codi
