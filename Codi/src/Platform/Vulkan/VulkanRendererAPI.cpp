#include "codipch.h"
#include "VulkanRendererAPI.h"

#include "Codi/Core/Application.h"

namespace Codi {
    Shared<VulkanGraphicsContext> VulkanRendererAPI::_Context = nullptr;

    void VulkanRendererAPI::Init() {
        Window* window = Application::Get().GetWindow();

        // Graphics Context
        VulkanDeviceRequirements deviceReq{};
        _Context = Own<VulkanGraphicsContext>(window->GetNativeWindow());
        _Context->Create(deviceReq);

        // Swapchain
        _Swapchain = Own<VulkanSwapchain>(_Context.get());
        _Swapchain->Create(window->GetWidth(), window->GetHeight());
        CODI_CORE_TRACE("Creating Swapchain of size {0}x{1}", window->GetWidth(), window->GetHeight());

    }

    void VulkanRendererAPI::Shutdown() {
        // Swapchain
        _Swapchain->Destroy();
        _Swapchain = nullptr;

        // Graphics Context
        _Context->Destroy();
        _Context = nullptr;
    }

} // namespace Codi
