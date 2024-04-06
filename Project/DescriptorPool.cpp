#include "DescriptorPool.h"
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>

DescriptorPool::DescriptorPool() {
}

const std::vector<VkDescriptorSet> &DescriptorPool::GetDescriptorSets() const {
    return descriptorSets;
}

//void DescriptorPool::CreateDescriptor()
//{
//	VkDescriptorSetLayoutBinding uboLayoutBinding{};
//	uboLayoutBinding.binding = 0;
//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//	uboLayoutBinding.descriptorCount = 1;
//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
//	uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
//
//	VkDescriptorSetLayoutCreateInfo layoutInfo{};
//	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
//	layoutInfo.bindingCount = 1;
//	layoutInfo.pBindings = &uboLayoutBinding;
//
//	if (vkCreateDescriptorSetLayout(VulkanBase::device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
//		throw std::runtime_error("failed to create descriptor set layout!");
//	}
//}

//void DescriptorPool::DestroyDescriptorSetLayout()
//{
//	vkDestroyDescriptorSetLayout(VulkanBase::device, m_DescriptorSetLayout, nullptr);
//}

void DescriptorPool::UpdateUniformBuffer(uint32_t currentFrame, UniformBufferObject ubo) {
//    float totalTime = TimeManager::GetInstance().GetTotalElapsed();
//
//    UniformBufferObject ubo{};
//    //ubo.model = glm::mat4(1);
//    ubo.model = glm::rotate(glm::mat4(1.0f), totalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//    ubo.proj = glm::perspective(glm::radians(45.0f),
//                                VulkanBase::swapChainExtent.width / (float) VulkanBase::swapChainExtent.height, 0.1f,
//                                10.0f);
//    ubo.proj[1][1] *= -1;
    memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}

void DescriptorPool::DestroyUniformBuffers() {
    //descriptor set layout must be destroyed before the buffer?
    //vkDestroyDescriptorSetLayout(VulkanBase::device, m_DescriptorSetLayout, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroyBuffer(VulkanBase::device, uniformBuffers[i], nullptr);
        vkFreeMemory(VulkanBase::device, uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorPool(VulkanBase::device, m_DescriptorPool, nullptr);
}

void DescriptorPool::CreateUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		DataBuffer::CreateBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
			uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(VulkanBase::device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void DescriptorPool::CreateDescriptorPool() {
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    //TODO: someone needs to catch these exceptions mate
    if (vkCreateDescriptorPool(VulkanBase::device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void DescriptorPool::CreateDescriptorSets(VkDescriptorSetLayout layout) {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, layout);
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
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet descriptorWrite{};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = descriptorSets[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;

        descriptorWrite.pBufferInfo = &bufferInfo;
        descriptorWrite.pImageInfo = nullptr; // Optional
        descriptorWrite.pTexelBufferView = nullptr; // Optional

        vkUpdateDescriptorSets(VulkanBase::device, 1, &descriptorWrite, 0, nullptr);
    }
}

void DescriptorPool::BindDescriptorSet(VkCommandBuffer buffer, VkPipelineLayout layout, size_t index) {
    vkCmdBindDescriptorSets(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, layout, 0,
                            1, &descriptorSets[index], 0, nullptr);
}

void DescriptorPool::Initialize(VkDescriptorSetLayout layout) {
    CreateUniformBuffers();
    CreateDescriptorPool();
    CreateDescriptorSets(layout);
}

