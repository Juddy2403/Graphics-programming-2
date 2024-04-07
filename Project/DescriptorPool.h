#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
//to fix the alignment requirements most of the time
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Vertex.h"


class DescriptorPool
{
private:
    //static VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorPool m_DescriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;

    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets(VkDescriptorSetLayout layout);
public:

    DescriptorPool();
    void Initialize(VkDescriptorSetLayout layout);
    [[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const;
    //void CreateDescriptor();
    //void DestroyDescriptorSetLayout();
    void UpdateUniformBuffer(uint32_t currentFrame, UniformBufferObject ubo) const;

    void DestroyUniformBuffers();

};
