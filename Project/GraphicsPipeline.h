#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

class Shader;
class GraphicsPipeline
{
private:
    VkPipeline m_GraphicsPipeline;
    static VkPipelineLayout m_PipelineLayout;
public:
    static VkPipelineLayout GetPipelineLayout() { return m_PipelineLayout; }

    VkPipeline& GetGraphicsPipeline() { return m_GraphicsPipeline; }
    void CreateGraphicsPipeline(const VkRenderPass &renderPass, Shader &gradientShader,
                                VkPipelineVertexInputStateCreateInfo pipelineVerInputStateCreateInfo,
                                bool enableDepthBuffering = true);
    void DestroyGraphicsPipeline();

    static void DestroyGraphicsPipelineLayout() ;

    static void CreatePipelineLayout() ;
};

