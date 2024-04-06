#pragma once
#include "Mesh.h"
#include <memory>

class Mesh;
class Level
{
private:
	std::vector<std::unique_ptr<Mesh>> m_Meshes;

public:
	void Update(uint32_t currentFrame);
	void initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void destroyLevel();
	void drawLevelMeshes(const VkCommandBuffer& commandBuffer, uint32_t currentFrame, const Descriptor& descriptor) const;
};
