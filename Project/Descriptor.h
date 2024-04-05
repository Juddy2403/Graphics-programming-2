#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

class Descriptor
{
private:
    static VkDescriptorSetLayout m_DescriptorSetLayout;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;
    std::vector<void*> uniformBuffersMapped;
    VkDescriptorPool m_DescriptorPool;

    std::vector<VkDescriptorSet> descriptorSets;

    void CreateUniformBuffers();
    void CreateDescriptorPool();
    void CreateDescriptorSets();
public:
    static const VkDescriptorSetLayout& GetDescriptorSetLayout();
    const std::vector<VkDescriptorSet>& GetDescriptorSets() const;
    void CreateDescriptorSetLayout();
    void DestroyDescriptor();
    void UpdateUniformBuffer(uint32_t currentFrame);

    void CreateDescriptor();
    void DestroyUniformBuffers();

};
