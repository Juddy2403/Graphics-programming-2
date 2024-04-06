//#include "VertexBuffers.h"
//#include <vulkanbase/VulkanBase.h>
//
//void VertexBuffers::copyBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
//{
//	CommandBuffer commandBufferClass{ commandPool };
//
//	//TODO: create a separate command pool for short lived objects using the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag
//
//	commandBufferClass.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
//
//	VkBufferCopy copyRegion{};
//	copyRegion.srcOffset = 0; // Optional
//	copyRegion.dstOffset = 0; // Optional
//	copyRegion.size = size;
//	vkCmdCopyBuffer(commandBufferClass.GetVkCommandBuffer(), srcBuffer, dstBuffer, 1, &copyRegion);
//
//	commandBufferClass.EndRecording();
//
//	VkSubmitInfo submitInfo{};
//	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//	commandBufferClass.Submit(submitInfo);
//
//	vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//	vkQueueWaitIdle(graphicsQueue);
//
//	commandBufferClass.FreeCommandBuffer(commandPool);
//}
//
//VertexBuffers::VertexBuffers(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices)
//{
//	m_Vertices = std::move(vertices);
//	m_Indices = std::move(indices);
//}
//
//void VertexBuffers::AddVertex(const glm::vec2& pos, const glm::vec3& color)
//{
//	//TODO: Def replace pos with vec3
//	m_Vertices.push_back(Vertex{ {pos,0},color });
//	m_Indices.emplace_back(static_cast<uint16_t>(m_Indices.size()));
//}
//
//void VertexBuffers::AddVertex(float xPos, float yPos, const glm::vec3& color)
//{
//	AddVertex(glm::vec2{ xPos,yPos }, color);
//}
//
//void VertexBuffers::createVertexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
//
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//
//	VulkanBase::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//	void* data;
//	vkMapMemory(VulkanBase::device, stagingBufferMemory, 0, bufferSize, 0, &data);
//	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
//	vkUnmapMemory(VulkanBase::device, stagingBufferMemory);
//
//	/*createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT,
//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_VertexBuffer, m_VertexBufferMemory);*/
//
//	VulkanBase::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_VertexBuffer, m_VertexBufferMemory);
//
//	copyBuffer(commandPool, graphicsQueue, stagingBuffer, m_VertexBuffer, bufferSize);
//
//	vkDestroyBuffer(VulkanBase::device, stagingBuffer, nullptr);
//	vkFreeMemory(VulkanBase::device, stagingBufferMemory, nullptr);
//}
//
//void VertexBuffers::createIndexBuffer(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();
//
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//	VulkanBase::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//	void* data;
//	vkMapMemory(VulkanBase::device, stagingBufferMemory, 0, bufferSize, 0, &data);
//	memcpy(data, m_Indices.data(), (size_t)bufferSize);
//	vkUnmapMemory(VulkanBase::device, stagingBufferMemory);
//
//	VulkanBase::createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_IndexBuffer, m_IndexBufferMemory);
//
//	copyBuffer(commandPool, graphicsQueue, stagingBuffer, m_IndexBuffer, bufferSize);
//
//	vkDestroyBuffer(VulkanBase::device, stagingBuffer, nullptr);
//	vkFreeMemory(VulkanBase::device, stagingBufferMemory, nullptr);
//}
//
//void VertexBuffers::DestroyBuffers()
//{
//	DestroyVertexBuffer();
//	DestroyIndexBuffer();
//}
//
//void VertexBuffers::DestroyVertexBuffer()
//{
//	m_Vertices.clear();
//	vkDestroyBuffer(VulkanBase::device, m_VertexBuffer, nullptr);
//	vkFreeMemory(VulkanBase::device, m_VertexBufferMemory, nullptr);
//}
//
//void VertexBuffers::DestroyIndexBuffer()
//{
//	m_Indices.clear();
//	vkDestroyBuffer(VulkanBase::device, m_IndexBuffer, nullptr);
//	vkFreeMemory(VulkanBase::device, m_IndexBufferMemory, nullptr);
//}
//
//
