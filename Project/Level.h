#pragma once
#include "Mesh.h"

class Mesh;
class Level
{
private:
	std::vector<Mesh> m_Meshes{};

public:
	void Update(uint32_t currentFrame);
	void initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void destroyLevel();
	void drawLevelMeshes(const VkCommandBuffer& commandBuffer, uint32_t currentFrame) const;
};
