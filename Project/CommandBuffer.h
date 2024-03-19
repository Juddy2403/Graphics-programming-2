#pragma once
#include <vulkan/vulkan_core.h>

class CommandBuffer
{
private:
	VkCommandBuffer m_CommandBuffer{};
	void CreateCommandBuffer(const VkCommandPool& commandPool);

public:
	CommandBuffer() = default;
	explicit CommandBuffer(const VkCommandPool& commandPool);
	const VkCommandBuffer& GetVkCommandBuffer() const;
	void SetVkCommandBuffer(VkCommandBuffer buffer) { m_CommandBuffer = buffer;}

	void Reset() const;
	void BeginRecording() const;
	void EndRecording() const;
	void Submit(VkSubmitInfo& info) const;
};
