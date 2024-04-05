#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

void Mesh::Update(uint32_t currentFrame)
{
}

void Mesh::initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_VertexBuffers.createVertexBuffer(commandPool,graphicsQueue);
	m_VertexBuffers.createIndexBuffer(commandPool, graphicsQueue);
}

//void Mesh::initializeCircle(const glm::vec2& center, float radius, int nrOfSegments,
//	const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//	m_Vertices.clear();
//	m_Vertices.reserve(nrOfSegments * 3);
//	const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;
//
//	glm::vec2 pos1 = { radius + center.x , center.y };
//	static glm::vec3 white{ 1,1,1 };
//	for (int i = 1; i <= nrOfSegments; ++i) {
//		const float angle = angleIncrement * i;
//		glm::vec2 pos2 = { radius * cos(angle) + center.x, radius * sin(angle) + center.y };
//
//		// Add vertex positions to the vertex buffer
//		m_Vertices.push_back(Vertex{ pos2, white });
//		m_Vertices.push_back(Vertex{ center, white });
//		m_Vertices.push_back(Vertex{ pos1, white });
//		pos1 = pos2;
//	}
//	createVertexBuffer(commandPool, graphicsQueue);
//}
//
//void Mesh::initializeRect(float top, float left, float bottom, float right,const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//	m_Vertices.clear();
//	addRect(top, left, bottom, right, commandPool, graphicsQueue);
//}
//
//void Mesh::addRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
//{
//	m_Vertices.push_back(Vertex{ {left,bottom} });
//	m_Vertices.push_back(Vertex{ {right,top} });
//	m_Vertices.push_back(Vertex{ {left,top} });
//
//	m_Vertices.push_back(Vertex{ {left,bottom} });
//	m_Vertices.push_back(Vertex{ {right,bottom} });
//	m_Vertices.push_back(Vertex{ {right,top} });
//
//	createVertexBuffer(commandPool, graphicsQueue);
//}
//
//void Mesh::initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments,
//	const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {
//
//	m_Vertices.clear();
//
//	// Calculate the corner vertices
//	const std::vector<glm::vec2> corners = {
//		{right , top },     // Top-right     0
//		{left , top },      // Top-left      1
//		{left , bottom } ,  // Bottom-left   2
//		{right, bottom}     // Bottom-right  3
//	};
//
//	std::vector<glm::vec2> pos1;
//	std::vector<glm::vec2> pos2;
//
//	constexpr float rightAngle = glm::pi<float>() / 2.f;
//	const float angleIncrement = rightAngle / nrOfSegments;
//
//	for (size_t i = 0; i < 4; i++)
//	{
//		pos1.push_back({ corners[i].x + radius * cos((angleIncrement * 0 + rightAngle * i) ),
//			corners[i].y + radius * sin((angleIncrement * 0 + rightAngle * i) ) });
//	}
//	addRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x,commandPool,graphicsQueue);
//	addRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x,commandPool,graphicsQueue);
//
//	for (int j = 1; j <= nrOfSegments; ++j) {
//		pos2.clear();
//
//		for (size_t i = 0; i < 4; i++)
//		{
//			pos2.push_back({ corners[i].x + radius * cos((angleIncrement * j + rightAngle * i)),
//			corners[i].y + radius * sin((angleIncrement * j + rightAngle * i)) });
//
//			m_Vertices.push_back(Vertex{ pos1[i] });
//			m_Vertices.push_back(Vertex{ pos2[i] });
//			m_Vertices.push_back(Vertex{ corners[i] });
//
//		}
//		pos1.clear();
//		pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
//	}
//
//	createVertexBuffer(commandPool, graphicsQueue);
//}
//
//
//void Mesh::addVertex(const glm::vec2& pos, const glm::vec3& color)
//{
//	m_Vertices.push_back(Vertex{ pos,color });
//	//createVertexBuffer();
//}


void Mesh::draw(const VkCommandBuffer& commandBuffer,uint32_t currentFrame, const Descriptor& descriptor) const
{
	VkBuffer vertexBuffers[] = { m_VertexBuffers.GetVertexBuffer()};
	VkDeviceSize offsets[] = { 0 };
	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, m_VertexBuffers.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);
	//vkCmdDraw(commandBuffer, static_cast<uint32_t>(m_Vertices.size()), 1, 0, 0);
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
		GraphicsPipeline::m_PipelineLayout, 0, 1, &descriptor.GetDescriptorSets()[currentFrame], 0, nullptr);
	vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_VertexBuffers.GetNrOfIndices()), 1, 0, 0, 0);
}

void Mesh::destroyMesh()
{
	m_VertexBuffers.DestroyBuffers();
}
