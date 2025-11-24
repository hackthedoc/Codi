#include "codipch.h"
#include "VulkanPipeline.h"

#include "Codi/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Codi {

    void VulkanPipeline::Create(std::vector<VkVertexInputAttributeDescription> attributes, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, std::vector<VkPipelineShaderStageCreateInfo> stages, uint32 stride, bool isWireframe) {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();
        VkViewport viewport = api.GetViewport();
        VkRect2D scissor = api.GetScissor();

        // Viewport State
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        // Rasterizer
        VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo{};
        rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizerCreateInfo.depthClampEnable = VK_TRUE;
        rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        rasterizerCreateInfo.polygonMode = isWireframe ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
        rasterizerCreateInfo.lineWidth = 1.0f;
        rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizerCreateInfo.depthBiasEnable = VK_FALSE;
        rasterizerCreateInfo.depthBiasConstantFactor = 0.0f;
        rasterizerCreateInfo.depthBiasClamp = 0.0f;
        rasterizerCreateInfo.depthBiasSlopeFactor = 0.0f;

        // Multisampling
        // TODO: use multisampling
        VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo{};
        multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;
        multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        multisamplingCreateInfo.minSampleShading = 1.0f;
        multisamplingCreateInfo.pSampleMask = nullptr;
        multisamplingCreateInfo.alphaToCoverageEnable = VK_FALSE;
        multisamplingCreateInfo.alphaToOneEnable = VK_FALSE;

        // Depth & Stencil testing
        VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo{};
        depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencilCreateInfo.depthTestEnable = VK_TRUE;
        depthStencilCreateInfo.depthWriteEnable = VK_TRUE;
        depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;
        depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;
        depthStencilCreateInfo.stencilTestEnable = VK_FALSE;

        // Color Blend
        VkPipelineColorBlendAttachmentState colorBlendAttachmentState{};
        colorBlendAttachmentState.blendEnable = VK_TRUE;
        colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                                                   VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

        VkPipelineColorBlendStateCreateInfo colorBlendCreateInfo{};
        colorBlendCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlendCreateInfo.logicOpEnable = VK_FALSE;
        colorBlendCreateInfo.logicOp = VK_LOGIC_OP_COPY;
        colorBlendCreateInfo.attachmentCount = 1;
        colorBlendCreateInfo.pAttachments = &colorBlendAttachmentState;

        // Dynamic State
        std::array<VkDynamicState, 3> dynamicState = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
            VK_DYNAMIC_STATE_LINE_WIDTH
        };

        VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{};
        dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicStateCreateInfo.dynamicStateCount = (uint32)dynamicState.size();
        dynamicStateCreateInfo.pDynamicStates = dynamicState.data();

        // Vertex Input
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0; // Binding index
        bindingDescription.stride = stride;
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // Move to next data entry for each vertex

        // Attributes
        VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo{};
        vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
        vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputCreateInfo.vertexAttributeDescriptionCount = (uint32)attributes.size();
        vertexInputCreateInfo.pVertexAttributeDescriptions = attributes.data();

        // Input Assembly
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo{};
        inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

        // Pipeline Layout
        VkPipelineLayoutCreateInfo layoutCreateInfo{};
        layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

        // Descriptor set layouts
        layoutCreateInfo.setLayoutCount = (uint32)descriptorSetLayouts.size();
        layoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();

        // Create the pipeline layout
        VkResult result = vkCreatePipelineLayout(logicalDevice, &layoutCreateInfo, VulkanRendererAPI::GetAllocator(), &_Layout);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan pipeline layout.");

        // Pipeline
        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.stageCount = (uint32)stages.size();
        pipelineCreateInfo.pStages = stages.data();
        pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;

        pipelineCreateInfo.pViewportState = &viewportState;
        pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
        pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
        pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
        pipelineCreateInfo.pColorBlendState = &colorBlendCreateInfo;
        pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
        pipelineCreateInfo.pTessellationState = nullptr;

        pipelineCreateInfo.layout = _Layout;

        pipelineCreateInfo.renderPass = api.GetMainRenderPass()->GetHandle();
        pipelineCreateInfo.subpass = 0;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineCreateInfo.basePipelineIndex = -1;

        result = vkCreateGraphicsPipelines(logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, VulkanRendererAPI::GetAllocator(), &_Handle);
        CODI_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan pipeline.");

        CODI_CORE_INFO("Vulkan graphics pipeline created!");
    }

    void VulkanPipeline::Destroy() {
        VulkanRendererAPI& api = static_cast<VulkanRendererAPI&>(Renderer::GetRAPI());
        VkDevice logicalDevice = api.GetContext()->GetLogicalDevice();

        if (_Handle)
            vkDestroyPipeline(logicalDevice, _Handle, VulkanRendererAPI::GetAllocator());

        if (_Layout)
            vkDestroyPipelineLayout(logicalDevice, _Layout, VulkanRendererAPI::GetAllocator());

        _Handle = VK_NULL_HANDLE;
        _Layout = VK_NULL_HANDLE;
    }

    void VulkanPipeline::Bind(VulkanCommandBuffer* commandBuffer, VkPipelineBindPoint bindPoint) {
        vkCmdBindPipeline(commandBuffer->GetHandle(), bindPoint, _Handle);
    }

} // namespace Codi
