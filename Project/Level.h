#pragma once
#include "Mesh.h"

class Mesh;
class Level
{
private:
	std::vector<Mesh> m_Meshes{};
	VkDevice m_VkDevice{};
	VkPhysicalDevice m_PhysicalDevice{};
public:
	void initializeLevel(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice);
	void destroyLevel();
	void drawLevelMeshes(const VkCommandBuffer& commandBuffer) const;
};
