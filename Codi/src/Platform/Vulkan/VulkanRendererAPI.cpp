#include "codipch.h"
#include "VulkanRendererAPI.h"

#include "Codi/Core/Application.h"

namespace Codi {

    void VulkanRendererAPI::Init() {
        Window* window = Application::Get().GetWindow();

        // Graphics Context
        VulkanDeviceRequirements deviceReq{};
        _Context = Own<VulkanGraphicsContext>(window->GetNativeWindow());
        _Context->Create(deviceReq);
    }

    void VulkanRendererAPI::Shutdown() {

        _Context->Destroy();
        _Context = nullptr;
    }

} // namespace Codi
