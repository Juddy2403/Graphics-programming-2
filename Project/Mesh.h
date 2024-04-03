#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <array>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec2 m_Pos;
	glm::vec3 m_Color{ 1,1,1 };

	static VkVertexInputBindingDescription getBindingDescription() {

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, m_Pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2; 
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, m_Color);

		return attributeDescriptions;
	}
};

class Mesh
{
private:
	std::vector<Vertex> m_Vertices = {
		 {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
	};
	const std::vector<uint16_t> m_Indices = {
	0, 1, 2, 2, 3, 0
	};
	//TODO: optionally store both vertex and index buffers in one VkBuffer and use offsets in commands
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	//TODO: encapsulate the buffer functionality
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void createVertexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void createIndexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void copyBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
public:
	Mesh() = default;

	void initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeCircle(const glm::vec2& center, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void addRect(float top, float left, float bottom, float right,const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void addVertex(const glm::vec2& pos, const glm::vec3& color);
	static VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();

	void draw(const VkCommandBuffer& commandBuffer) const;
	void destroyMesh();
};
