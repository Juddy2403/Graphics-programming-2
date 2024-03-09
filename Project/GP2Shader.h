#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <array>
#include <glm/glm.hpp>

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

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
		attributeDescriptions[0].offset = offsetof(Vertex, pos);
		
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 2; //shouldnt this be 2?
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

class GP2Shader
{
public:
	GP2Shader(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	) : m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile }
	{}
	~GP2Shader() = default;
	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages()
	{
		return m_ShaderStages;
	}

	void Initialize(const VkDevice& vkDevice);
	void DestroyShaderModules(const VkDevice& vkDevice);
	VkPipelineVertexInputStateCreateInfo createVertexInputStateInfo();
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo(const VkDevice& vkDevice);
	VkPipelineShaderStageCreateInfo createVertexShaderInfo(const VkDevice& vkDevice);
	void createVertexBuffer(const VkDevice& vkDevice, const VkPhysicalDevice& physicalDevice);
	VkBuffer& getVertexBuffer() { return m_VertexBuffer; }
	VkDeviceMemory& getVertexBufferMemory() { return m_VertexBufferMemory; }
	const std::vector<Vertex>& getVertices() { return m_Vertices; };
private:
	const std::vector<Vertex> m_Vertices = {
		{{0.0f, -0.5f}, {1.0f, 1.0f, 1.0f}},
	{{0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}},
	{{-0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}}
	};
	VkBuffer m_VertexBuffer{};
	VkDeviceMemory m_VertexBufferMemory{};

	VkShaderModule createShaderModule(const VkDevice& vkDevice, const std::vector<char>& code);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, const VkPhysicalDevice& physicalDevice);
	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator=(const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator=(const GP2Shader&&) = delete;
};
