#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

void Mesh::initializeMesh()
{
	createVertexBuffer();
}

void Mesh::initializeCircle(const glm::vec2& center, float radius, int nrOfSegments)
{
	m_Vertices.clear();
	m_Vertices.reserve(nrOfSegments * 3);
	const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;

	glm::vec2 pos1 = { radius + center.x , center.y };
	static glm::vec3 white{ 1,1,1 };
	for (int i = 1; i <= nrOfSegments; ++i) {
		const float angle = angleIncrement * i;
		glm::vec2 pos2 = { radius * cos(angle) + center.x, radius * sin(angle) + center.y };

		// Add vertex positions to the vertex buffer
		m_Vertices.push_back(Vertex{ pos2, white });
		m_Vertices.push_back(Vertex{ center, white });
		m_Vertices.push_back(Vertex{ pos1, white });
		pos1 = pos2;
	}
	createVertexBuffer();
}

void Mesh::initializeRect(float top, float left, float bottom, float right)
{
	m_Vertices.clear();
	addRect(top, left, bottom, right);
}

void Mesh::addRect(float top, float left, float bottom, float right)
{
	m_Vertices.push_back(Vertex{ {left,bottom} });
	m_Vertices.push_back(Vertex{ {right,top} });
	m_Vertices.push_back(Vertex{ {left,top} });

	m_Vertices.push_back(Vertex{ {left,bottom} });
	m_Vertices.push_back(Vertex{ {right,bottom} });
	m_Vertices.push_back(Vertex{ {right,top} });

	createVertexBuffer();
}

void Mesh::initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments) {

	m_Vertices.clear();

	// Calculate the corner vertices
	const std::vector<glm::vec2> corners = {
		{right , top },     // Top-right     0
		{left , top },      // Top-left      1
		{left , bottom } ,  // Bottom-left   2
		{right, bottom}     // Bottom-right  3
	};

	std::vector<glm::vec2> pos1;
	std::vector<glm::vec2> pos2;

	constexpr float rightAngle = glm::pi<float>() / 2.f;
	const float angleIncrement = rightAngle / nrOfSegments;

	for (size_t i = 0; i < 4; i++)
	{
		pos1.push_back({ corners[i].x + radius * cos((angleIncrement * 0 + rightAngle * i) ),
			corners[i].y + radius * sin((angleIncrement * 0 + rightAngle * i) ) });
	}
	addRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x);
	addRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x);

	for (int j = 1; j <= nrOfSegments; ++j) {
		pos2.clear();

		for (size_t i = 0; i < 4; i++)
		{
			pos2.push_back({ corners[i].x + radius * cos((angleIncrement * j + rightAngle * i)),
			corners[i].y + radius * sin((angleIncrement * j + rightAngle * i)) });

			m_Vertices.push_back(Vertex{ pos1[i] });
			m_Vertices.push_back(Vertex{ pos2[i] });
			m_Vertices.push_back(Vertex{ corners[i] });

		}
		pos1.clear();
		pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
	}

	createVertexBuffer();
}

VkPipelineVertexInputStateCreateInfo Mesh::createVertexInputStateInfo()
{
	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

	static auto bindingDescription = Vertex::getBindingDescription();
	static auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	//vertexInputInfo.vertexBindingDescriptionCount = 0; //&m_VertexInputBindingDescription
	//vertexInputInfo.vertexAttributeDescriptionCount = 0;

	return vertexInputInfo;
}

void Mesh::addVertex(const glm::vec2& pos, const glm::vec3& color)
{
	m_Vertices.push_back(Vertex{ pos,color });
	//createVertexBuffer();
}

uint32_t Mesh::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(VulkanBase::physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");

	return 0;
}


void Mesh::createVertexBuffer()
{
	destroyMesh();
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(m_Vertices[0]) * m_Vertices.size();
	bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(VulkanBase::device, &bufferInfo, nullptr, &m_VertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create vertex buffer!");
	}
	VkMemoryRequirements memRequirements{};

	vkGetBufferMemoryRequirements(VulkanBase::device, m_VertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(VulkanBase::device, &allocInfo, nullptr, &m_VertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(VulkanBase::device, m_VertexBuffer, m_VertexBufferMemory, 0); //0 is the offset in memory. If its not 0 it needs to be divisible by memRequirements.alignment

	void* data;
	vkMapMemory(VulkanBase::device, m_VertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferInfo.size);
	vkUnmapMemory(VulkanBase::device, m_VertexBufferMemory);
}

void Mesh::draw(const VkCommandBuffer& commandBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

	vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);

}

void Mesh::destroyMesh()
{
	vkDestroyBuffer(VulkanBase::device, m_VertexBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, m_VertexBufferMemory, nullptr);
}
