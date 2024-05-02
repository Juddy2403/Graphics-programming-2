#include "DescriptorPool.h"
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>

const std::vector<VkDescriptorSet> &DescriptorPool::GetDescriptorSets() const {
    return descriptorSets;
}

void DescriptorPool::UpdateUniformBuffer(uint32_t currentFrame, UniformBufferObject ubo) {
    memcpy(m_UniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}

void DescriptorPool::DestroyUniformBuffers() {
    //descriptor set layout must be destroyed before the buffer?
    //vkDestroyDescriptorSetLayout(VulkanBase::device, m_DescriptorSetLayout, nullptr);
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(VulkanBase::device, m_UniformBuffers[i], nullptr);
        vkFreeMemory(VulkanBase::device, m_UniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(VulkanBase::device, m_DescriptorPool, nullptr);
}

void DescriptorPool::CreateUniformBuffers() {
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_UniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_UniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    m_UniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        DataBuffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                 m_UniformBuffers[i], m_UniformBuffersMemory[i]);

        vkMapMemory(VulkanBase::device, m_UniformBuffersMemory[i], 0, bufferSize, 0, &m_UniformBuffersMapped[i]);
    }
}

void DescriptorPool::CreateDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    //TODO: someone needs to catch these exceptions mate
    if (vkCreateDescriptorPool(VulkanBase::device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorPool::CreateDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, Shader::GetDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_DescriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(VulkanBase::device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        UpdateDescriptorSets(i);
    }
}

void DescriptorPool::UpdateDescriptorSets(uint32_t currentFrame) {
    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_UniformBuffers[currentFrame];
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(UniformBufferObject);

    if (m_ImageView == VK_NULL_HANDLE) {
        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[currentFrame];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional
        vkUpdateDescriptorSets(VulkanBase::device, 1, &descriptorWrite, 0, nullptr);
    } else {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_ImageView;
        imageInfo.sampler = Texture::GetTextureSampler();

        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = descriptorSets[currentFrame];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;

        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = descriptorSets[currentFrame];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        vkUpdateDescriptorSets(VulkanBase::device, static_cast<uint32_t>(descriptorWrites.size()),
                               descriptorWrites.data(), 0, nullptr);
    }
}


void DescriptorPool::Initialize(VkImageView imageView) {
    CreateUniformBuffers();
    CreateDescriptorPool();
    m_ImageView = imageView;
    CreateDescriptorSets();
}

