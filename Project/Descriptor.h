#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
//to fix the alignment requirements most of the time
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
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
