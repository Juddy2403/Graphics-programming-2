#include "Level.h"
#include <vulkanbase/VulkanUtil.h>

void Level::initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_Meshes.push_back(Mesh());
	for (auto& meshes : m_Meshes)
	{
		meshes.initializeMesh(commandPool, graphicsQueue);
	}
	//m_Meshes[0].initializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20, commandPool, graphicsQueue);

}

void Level::destroyLevel()
{
	for (auto& meshes : m_Meshes)
	{
		meshes.destroyMesh();
	}
}

void Level::drawLevelMeshes(const VkCommandBuffer& commandBuffer) const
{
	for (const auto& meshes : m_Meshes)
	{
		meshes.draw(commandBuffer);
	}
}
