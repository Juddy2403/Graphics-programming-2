#include "Level.h"
#include <vulkanbase/VulkanUtil.h>

void Level::Update(uint32_t currentFrame)
{
	for (auto& mesh : m_Meshes)
	{
		mesh.Update(currentFrame);
	}
}

void Level::initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_Meshes.push_back(Mesh());
	for (auto& mesh : m_Meshes)
	{
		mesh.initializeMesh(commandPool, graphicsQueue);
	}
	//m_Meshes[0].initializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20, commandPool, graphicsQueue);
	//m_Meshes[0].initializeRect(-0.3, 0.3, 0.3, -0.3, commandPool, graphicsQueue);
	//m_Meshes[0].initializeCircle({0,0},1,30, commandPool, graphicsQueue);

}

void Level::destroyLevel()
{
	for (auto& mesh : m_Meshes)
	{
		mesh.destroyMesh();
	}
}

void Level::drawLevelMeshes(const VkCommandBuffer& commandBuffer, uint32_t currentFrame, const Descriptor& descriptor) const
{
	for (const auto& mesh : m_Meshes)
	{
		mesh.draw(commandBuffer,currentFrame,descriptor);
	}
}
