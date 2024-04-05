#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

class GP2Shader;
class GraphicsPipeline
{
private:
    VkPipeline m_GraphicsPipeline;
public:
    //TODO: yeah dont do that
    static VkPipelineLayout m_PipelineLayout;

    VkPipeline& getGraphicsPipeline() { return m_GraphicsPipeline; }
    void createGraphicsPipeline(const VkRenderPass& renderPass,
                                      GP2Shader& gradientShader);
    void destroyGraphicsPipeline();
};

