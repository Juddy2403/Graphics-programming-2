#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

void Mesh::Update(uint32_t currentFrame)
{
	UpdateUniformBuffer(currentFrame);
}

void Mesh::UpdateUniformBuffer(uint32_t currentFrame)
{
	m_TotalTime += TimeManager::GetInstance().GetElapsed();
	
	UniformBufferObject ubo{};
	//TODO: CHECK! this probably uses the total time not the elapsed sec
	ubo.model = glm::rotate(glm::mat4(1.0f), m_TotalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), VulkanBase::swapChainExtent.width / (float)VulkanBase::swapChainExtent.height, 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;
	memcpy(uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));
}

void Mesh::initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	createVertexBuffer(commandPool,graphicsQueue);
	createIndexBuffer(commandPool, graphicsQueue);
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
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

void Mesh::createUniformBuffers()
{
	//TODO: this should be in a separate class! it is NOT mesh specific
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
	uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			uniformBuffers[i], uniformBuffersMemory[i]);

		vkMapMemory(VulkanBase::device, uniformBuffersMemory[i], 0, bufferSize, 0, &uniformBuffersMapped[i]);
	}
}

void Mesh::createDescriptorPool()
{
	VkDescriptorPoolSize poolSize{};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	//TODO: someone needs to catch these exceptions mate
	if (vkCreateDescriptorPool(VulkanBase::device, &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Mesh::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, GraphicsPipeline::m_DescriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
	if (vkAllocateDescriptorSets(VulkanBase::device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;

		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;

		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(VulkanBase::device, 1, &descriptorWrite, 0, nullptr);
	}
}

void Mesh::copyBuffer(const VkCommandPool& commandPool,const VkQueue& graphicsQueue,VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {

	CommandBuffer commandBufferClass{commandPool};
	
	//TODO: create a separate command pool for short lived objects using the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag

	commandBufferClass.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkBufferCopy copyRegion{};
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBufferClass.GetVkCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);

	commandBufferClass.EndRecording();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	commandBufferClass.Submit(submitInfo);

	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(graphicsQueue);

	commandBufferClass.FreeCommandBuffer(commandPool);
}

void Mesh::draw(const VkCommandBuffer& commandBuffer,uint32_t currentFrame) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffer };
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, m_IndexBuffer, 0, VK_INDEX_TYPE_UINT16);
	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		GraphicsPipeline::m_PipelineLayout, 0, 1, &descriptorSets[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

}

void Mesh::destroyMesh()
{
	vkDestroyBuffer(VulkanBase::device, m_VertexBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, m_VertexBufferMemory, nullptr);

	vkDestroyBuffer(VulkanBase::device, m_IndexBuffer, nullptr);
	vkFreeMemory(VulkanBase::device, m_IndexBufferMemory, nullptr);

}

void Mesh::destroyUniformBuffers()
{
	//TODO: this needs to be destroyed right before descriptorSetLayout
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyBuffer(VulkanBase::device, uniformBuffers[i], nullptr);
		vkFreeMemory(VulkanBase::device, uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(VulkanBase::device, m_DescriptorPool, nullptr);

}
