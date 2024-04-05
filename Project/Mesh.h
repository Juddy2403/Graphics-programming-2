#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>

#include "Descriptor.h"
#include <VertexBuffers.h>

class Descriptor;
class Mesh
{
private:
	VertexBuffers m_VertexBuffers{};

public:
	Mesh() = default;
	void Update(uint32_t currentFrame);
	void initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	/*void initializeCircle(const glm::vec2& center, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void addRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void addVertex(const glm::vec2& pos, const glm::vec3& color);*/

	void draw(const VkCommandBuffer& commandBuffer, uint32_t currentFrame, const Descriptor& descriptor) const;
	void destroyMesh();
};
