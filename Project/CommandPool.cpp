#include "CommandPool.h"
#include "vulkanbase/VulkanBase.h"

VkCommandPool CommandPool::CreateCommandPool(const VkSurfaceKHR& surface, const QueueFamilyIndices& queueFamilyIndices,
                                             VkCommandPoolCreateFlags flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT) {
	VkCommandPool commandPool{};
	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = flags;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(VulkanBase::device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}

	return commandPool;
}

void CommandPool::DestroyCommandPool()
{
	vkDestroyCommandPool(VulkanBase::device, m_CommandPool, nullptr);
}

CommandPool::CommandPool(const VkSurfaceKHR& surface, const QueueFamilyIndices& queueFamilyIndices)
{
	m_CommandPool = CreateCommandPool(surface, queueFamilyIndices);
}

const VkCommandPool& CommandPool::GetCommandPool()
{
	return m_CommandPool;
}

