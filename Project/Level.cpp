#include "Level.h"
#include <vulkanbase/VulkanUtil.h>

void Level::initializeLevel(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice)
{
	m_VkDevice = vkDevice;
	m_PhysicalDevice = physicalDevice;

	m_Meshes.push_back(std::make_unique<Mesh>());
	for (auto& meshes : m_Meshes)
	{
		meshes->initializeMesh(vkDevice, physicalDevice);
	}
	m_Meshes[0]->initializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20);

}

void Level::drawLevelMeshes(const VkCommandBuffer& commandBuffer) const
{
	for (const auto& meshes : m_Meshes)
	{
		meshes->draw(commandBuffer);
	}
}
