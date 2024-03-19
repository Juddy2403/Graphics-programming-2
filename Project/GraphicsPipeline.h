#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

class GP2Shader;
class GraphicsPipeline
{
private:
    VkPipelineLayout m_PipelineLayout;
    VkPipeline m_GraphicsPipeline;
public:
    VkPipeline& getGraphicsPipeline() { return m_GraphicsPipeline; }
    void createGraphicsPipeline(const VkRenderPass& renderPass,
                                      GP2Shader& gradientShader);
    void destroyGraphicsPipeline();
};
