#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <array>

//to fix the alignment requirements most of the time
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct Vertex {
	//should be a vec3
	alignas(16) glm::vec3 m_Pos;
	//glm::vec2 m_TexCoord;
	//glm::vec3 m_Normal;
	//glm::vec3 m_Tangent;
	alignas(16) glm::vec3 m_Color{ 1,1,1 };

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
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, m_Pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, m_Color);

		return attributeDescriptions;
	}
};

class VertexBuffers
{
private:
	std::vector<Vertex> m_Vertices = {
	{{-0.5f, -0.5f,0.f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f,0.f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f,0.f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f,0.f}, {1.0f, 1.0f, 1.0f}}
	};
	std::vector<uint16_t> m_Indices = { 0, 1, 2, 2, 3, 0 };
	//TODO: optionally store both vertex and index buffers in one VkBuffer and use offsets in commands
	//vertex and index buffers are unique to graphics pipelines, descriptor sets are not
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};
	VkBuffer m_IndexBuffer;
	VkDeviceMemory m_IndexBufferMemory;


	void copyBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

public:
	VertexBuffers() = default;
	VertexBuffers(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices);
	size_t GetNrOfIndices() const { return m_Indices.size(); }
	const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
	const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
	void AddVertex(const glm::vec2& pos, const glm::vec3& color = { 1,1,1 });
	void AddVertex(float xPos, float yPos, const glm::vec3& color = { 1,1,1 });
	void createVertexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void createIndexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void DestroyBuffers();
	void DestroyVertexBuffer();
	void DestroyIndexBuffer();

	static VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();

};
