#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
//to fix the alignment requirements most of the time
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>
#include "Vertex.h"


class DescriptorPool final
{
private:
    //static VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkBuffer> m_UniformBuffers;
    std::vector<VkDeviceMemory> m_UniformBuffersMemory;
    std::vector<void*> m_UniformBuffersMapped;
    VkDescriptorPool m_DescriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;
    VkImageView m_ImageView;

    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
public:

    DescriptorPool() = default;
    void Initialize(VkImageView imageView = VK_NULL_HANDLE);
    [[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const;
    //void CreateDescriptor();
    //void DestroyDescriptorSetLayout();
    void UpdateUniformBuffer(uint32_t currentFrame, UniformBufferObject ubo);

    void DestroyUniformBuffers();

    void UpdateDescriptorSets(uint32_t currentFrame);
};
