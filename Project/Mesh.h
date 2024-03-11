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
	VkDevice m_VkDevice{};
	VkPhysicalDevice m_PhysicalDevice{};
	std::vector<Vertex> m_Vertices = {
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}

	};
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

public:
	Mesh() = default;

	void initializeMesh(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice);
	void initializeCircle(const glm::vec2& center, float radius, int nrOfSegments);
	void initializeRect(float top, float left, float bottom, float right);
	void addRect(float top, float left, float bottom, float right);
	void initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments);
	void addVertex(const glm::vec2& pos, const glm::vec3& color);

	static VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(); 
	void createVertexBuffer();
	void draw(const VkCommandBuffer& commandBuffer) const;
	void destroyMesh();
};
