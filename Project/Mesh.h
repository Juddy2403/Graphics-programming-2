#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <array>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec2 m_Pos;
	glm::vec3 m_Color;

	Vertex(const glm::vec2& pos, const glm::vec3& color) :
		m_Pos{ pos }, m_Color{ color } {};

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
		{{0.0f, -0.5f}, {1.0f, 0.0f, 0.0f}},
		{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
		{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},

		{{-0.6f, -0.1f}, {1.0f, 1.0f, 1.0f}},
		{{0.1f, 0.3f}, {1.0f, 1.0f, 1.0f}},
		{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}},
	};
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice);

public:
	Mesh() = default;
	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo(); 
	void addVertex(const glm::vec2& pos, const glm::vec3& color);
	void createVertexBuffer(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice);
	void draw(const VkCommandBuffer& commandBuffer) const;
	void destroyMesh(const VkDevice& vkDevice);
};
