#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

void Mesh::initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	createVertexBuffer(commandPool,graphicsQueue);
	createIndexBuffer(commandPool, graphicsQueue);
}

void Mesh::initializeCircle(const glm::vec2& center, float radius, int nrOfSegments,
	const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
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
	createVertexBuffer(commandPool, graphicsQueue);
}

void Mesh::initializeRect(float top, float left, float bottom, float right,const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_Vertices.clear();
	addRect(top, left, bottom, right, commandPool, graphicsQueue);
}

void Mesh::addRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_Vertices.push_back(Vertex{ {left,bottom} });
	m_Vertices.push_back(Vertex{ {right,top} });
	m_Vertices.push_back(Vertex{ {left,top} });

	m_Vertices.push_back(Vertex{ {left,bottom} });
	m_Vertices.push_back(Vertex{ {right,bottom} });
	m_Vertices.push_back(Vertex{ {right,top} });

	createVertexBuffer(commandPool, graphicsQueue);
}

void Mesh::initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments,
	const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {

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
	addRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x,commandPool,graphicsQueue);
	addRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x,commandPool,graphicsQueue);

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

	createVertexBuffer(commandPool, graphicsQueue);
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

void Mesh::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(VulkanBase::device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(VulkanBase::device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(VulkanBase::device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(VulkanBase::device, buffer, bufferMemory, 0); //0 is the offset in memory. If its not 0 it needs to be divisible by memRequirements.alignment
}

void Mesh::createVertexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	destroyMesh();
	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(VulkanBase::device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(VulkanBase::device, stagingBufferMemory);

	/*createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_VertexBuffer, m_VertexBufferMemory);*/

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);

	copyBuffer( commandPool, graphicsQueue,stagingBuffer, m_VertexBuffer, bufferSize);

	vkDestroyBuffer(VulkanBase::device, stagingBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, stagingBufferMemory, nullptr);
}

void Mesh::createIndexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(VulkanBase::device, stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), (size_t)bufferSize);
	vkUnmapMemory(VulkanBase::device, stagingBufferMemory);

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);

	copyBuffer(commandPool, graphicsQueue, stagingBuffer, m_IndexBuffer, bufferSize);

	vkDestroyBuffer(VulkanBase::device, stagingBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, stagingBufferMemory, nullptr);
}

void Mesh::copyBuffer(const VkCommandPool& commandPool,const VkQueue& graphicsQueue,VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
	//TODO: encapsulate this part within the command buffer class
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//TODO: create a separate command pool for short lived objects using the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag
	allocInfo.commandPool = commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(VulkanBase::device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	vkFreeCommandBuffers(VulkanBase::device, commandPool, 1, &commandBuffer);
}

void Mesh::draw(const VkCommandBuffer& commandBuffer) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

}

void Mesh::destroyMesh()
{
	vkDestroyBuffer(VulkanBase::device, m_VertexBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, m_VertexBufferMemory, nullptr);

	vkDestroyBuffer(VulkanBase::device, m_IndexBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, m_IndexBufferMemory, nullptr);
}
