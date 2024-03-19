#pragma once
#include <vulkan/vulkan_core.h>

class CommandBuffer
{
private:
	VkCommandBuffer m_CommandBuffer{};
	void createCommandBuffer(const VkCommandPool& commandPool);

public:
	CommandBuffer() = default;
	explicit CommandBuffer(const VkCommandPool& commandPool);
	const VkCommandBuffer& GetVkCommandBuffer();
};
