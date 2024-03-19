#pragma once
#include "Mesh.h"

class Mesh;
class Level
{
private:
	std::vector<Mesh> m_Meshes{};

public:
	void initializeLevel();
	void destroyLevel();
	void drawLevelMeshes(const VkCommandBuffer& commandBuffer) const;
};
