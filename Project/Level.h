#pragma once
#include "Mesh.h"

class Mesh;
class Level
{
private:
	std::vector<Mesh> m_Meshes{};

public:
	void initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void destroyLevel();
	void drawLevelMeshes(const VkCommandBuffer& commandBuffer) const;
};
