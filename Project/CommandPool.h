#pragma once
#include <vulkan/vulkan_core.h>
#include <optional>

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};
class CommandPool
{
public:
	CommandPool() = default;
	void DestroyCommandPool();
	explicit CommandPool(const VkSurfaceKHR& surface, const QueueFamilyIndices& queueFamilyIndices);
	const VkCommandPool& GetCommandPool();
private:
	VkCommandPool createCommandPool(const VkSurfaceKHR& surface, const QueueFamilyIndices& queueFamilyIndices, VkCommandPoolCreateFlags flags);
	VkCommandPool m_CommandPool{};

};
