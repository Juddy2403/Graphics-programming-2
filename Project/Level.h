#pragma once
#include "Mesh.h"
#include <memory>

class Mesh;
class Level
{
private:
	std::vector<std::unique_ptr<Mesh>> m_Meshes{};
	VkDevice m_VkDevice;
	VkPhysicalDevice m_PhysicalDevice;
public:
	void initializeLevel(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice);

	void drawLevelMeshes(const VkCommandBuffer& commandBuffer) const;
};
