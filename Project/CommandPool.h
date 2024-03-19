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
	explicit CommandPool(const VkSurfaceKHR& surface);
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	VkCommandPool& GetCommandPool();
private:
	void createCommandPool(const VkSurfaceKHR& surface);
	VkCommandPool m_CommandPool{};

};
