//#pragma once
//#include <vulkan/vulkan_core.h>
//#include <vector>
//#include <array>
//#include "Vertex.h"
////to fix the alignment requirements most of the time
//#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
//#include <glm/glm.hpp>
//
//
//class VertexBuffers
//{
//private:
//	std::vector<Vertex> m_Vertices = {
//	{{-0.5f, -0.5f,0.f}, {1.0f, 0.0f, 0.0f}},
//	{{0.5f, -0.5f,0.f}, {0.0f, 1.0f, 0.0f}},
//	{{0.5f, 0.5f,0.f}, {0.0f, 0.0f, 1.0f}},
//	{{-0.5f, 0.5f,0.f}, {1.0f, 1.0f, 1.0f}}
//	};
//	std::vector<uint16_t> m_Indices = { 0, 1, 2, 2, 3, 0 };
//	//TODO: optionally store both vertex and index buffers in one VkBuffer and use offsets in commands
//	//vertex and index buffers are unique to graphics pipelines, descriptor sets are not
//	VkBuffer m_VertexBuffer{};
//	VkDeviceMemory m_VertexBufferMemory{};
//	VkBuffer m_IndexBuffer;
//	VkDeviceMemory m_IndexBufferMemory;
//
//
//	void copyBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
//
//public:
//	VertexBuffers() = default;
//	VertexBuffers(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices);
//	size_t GetNrOfIndices() const { return m_Indices.size(); }
//	const VkBuffer& GetVertexBuffer() const { return m_VertexBuffer; }
//	const VkBuffer& GetIndexBuffer() const { return m_IndexBuffer; }
//	void AddVertex(const glm::vec2& pos, const glm::vec3& color = { 1,1,1 });
//	void AddVertex(float xPos, float yPos, const glm::vec3& color = { 1,1,1 });
//	void createVertexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
//	void createIndexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
//	void DestroyBuffers();
//	void DestroyVertexBuffer();
//	void DestroyIndexBuffer();
//
//
//};
