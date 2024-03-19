#include "CommandBuffer.h"
#include "vulkanbase/VulkanBase.h"

CommandBuffer::CommandBuffer(const VkCommandPool& commandPool)
{
	m_CommandBuffer = CreateCommandBuffer(commandPool);
}

const VkCommandBuffer& CommandBuffer::GetVkCommandBuffer() const
{
	return m_CommandBuffer;
}

void CommandBuffer::Reset() const
{
	vkResetCommandBuffer(m_CommandBuffer, /*VkCommandBufferResetFlagBits*/ 0);
}

void CommandBuffer::BeginRecording(const VkCommandBufferUsageFlags& flags) const
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = flags; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer(m_CommandBuffer, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");

	}
}

void CommandBuffer::EndRecording() const
{
	if (vkEndCommandBuffer(m_CommandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to record command buffer!");
	}
}

void CommandBuffer::Submit(VkSubmitInfo& info) const
{
	info.commandBufferCount = 1;
	info.pCommandBuffers = &m_CommandBuffer;
}

VkCommandBuffer CommandBuffer::CreateCommandBuffer(const VkCommandPool& commandPool) {

	VkCommandBuffer commandBuffer{};

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = 1;

	if (vkAllocateCommandBuffers(VulkanBase::device, &allocInfo, &commandBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}
	return commandBuffer;
}

void CommandBuffer::FreeCommandBuffer(const VkCommandPool& commandPool)
{
	vkFreeCommandBuffers(VulkanBase::device, commandPool, 1, &m_CommandBuffer);
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
