#pragma once
#include <vulkan/vulkan_core.h>

class CommandBuffer
{
private:
	VkCommandBuffer m_CommandBuffer{};
	void createCommandBuffer(const VkDevice& device, const VkCommandPool& commandPool);

public:
	CommandBuffer() = default;
	explicit CommandBuffer(const VkDevice& device, const VkCommandPool& commandPool);
	VkCommandBuffer& GetCommandBuffer();
};
