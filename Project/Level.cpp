#include "Level.h"
#include <vulkanbase/VulkanUtil.h>

void Level::Update(uint32_t currentFrame)
{
	for (auto& mesh : m_Meshes)
	{
		mesh->Update(currentFrame);
	}
}

void Level::initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	//m_Meshes.emplace_back(std::make_unique<Mesh>());
    std::vector<Vertex> vertices = {
        Vertex{{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
        Vertex{{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
        Vertex{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
        Vertex{{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint16_t> indices = { 0, 1, 2, 2, 3, 0 };
	m_Meshes.emplace_back(std::make_unique<Mesh>(std::move(vertices), std::move(indices)));

	for (auto& mesh : m_Meshes)
	{
        mesh->UploadMesh(commandPool, graphicsQueue);
	}
	//m_Meshes[0].initializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20, commandPool, graphicsQueue);
	//m_Meshes[0].initializeRect(-0.3, 0.3, 0.3, -0.3, commandPool, graphicsQueue);
	//m_Meshes[0].initializeCircle({0,0},1,30, commandPool, graphicsQueue);

}

void Level::destroyLevel()
{
    for (auto& mesh: m_Meshes)
        mesh->Destroy();

}

void Level::drawLevelMeshes(const VkCommandBuffer& commandBuffer, uint32_t currentFrame, const Descriptor& descriptor) const
{
	for (const auto& mesh : m_Meshes)
	{
		mesh->draw(commandBuffer,currentFrame,descriptor);
	}
}
