#pragma once
#include "3DMesh.h"
#include "2DMesh.h"
#include <memory>

class Mesh3D;
class Level
{
private:
	std::vector<std::unique_ptr<Mesh3D>> m_3DMeshes;
	std::vector<std::unique_ptr<Mesh2D>> m_2DMeshes;

public:
	void Update(uint32_t currentFrame);
	void initializeLevel(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void destroyLevel();
	void Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

    void Draw2DMeshes(VkCommandBuffer const &commandBuffer, uint32_t currentFrame) const;
};
