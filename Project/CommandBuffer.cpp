#include "CommandBuffer.h"
#include "vulkanbase/VulkanBase.h"

CommandBuffer::CommandBuffer(const VkCommandPool& commandPool)
{
	createCommandBuffer(commandPool);
}

VkCommandBuffer& CommandBuffer::GetCommandBuffer()
{
	return m_CommandBuffer;
}

void CommandBuffer::createCommandBuffer(const VkCommandPool& commandPool) {
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(VulkanBase::device, &allocInfo, &m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
}

//void CommandBuffer::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
//	VkCommandBufferBeginInfo beginInfo{};
//	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//	beginInfo.flags = 0; // Optional
//	beginInfo.pInheritanceInfo = nullptr; // Optional
//
//	if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
//		throw std::runtime_error("failed to begin recording command buffer!");
//	}
//	drawFrame(imageIndex);
//
//
//	if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
//		throw std::runtime_error("failed to record command buffer!");
//	}
//}
