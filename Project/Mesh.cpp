#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices):
	m_VertexBuffers{std::move(vertices),std::move(indices)}
{
}

void Mesh::Update(uint32_t currentFrame)
{
}

void Mesh::initializeMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_VertexBuffers.createVertexBuffer(commandPool,graphicsQueue);
	m_VertexBuffers.createIndexBuffer(commandPool, graphicsQueue);
}

void Mesh::initializeCircle(const glm::vec2& center, float radius, int nrOfSegments,
	const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	destroyMesh();

	const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;

	glm::vec2 pos1 = { radius + center.x , center.y };

	for (int i = 1; i <= nrOfSegments; ++i) {
		const float angle = angleIncrement * i;
		glm::vec2 pos2 = { radius * cos(angle) + center.x, radius * sin(angle) + center.y };

		// Add vertex positions to the vertex buffer
		m_VertexBuffers.AddVertex(pos2);
		m_VertexBuffers.AddVertex(center);
		m_VertexBuffers.AddVertex(pos1);
		pos1 = pos2;
	}

	initializeMesh(commandPool, graphicsQueue);
}

void Mesh::initializeRect(float top, float left, float bottom, float right,const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	destroyMesh();

	addRect(top, left, bottom, right, commandPool, graphicsQueue);

	initializeMesh(commandPool, graphicsQueue);
}

void Mesh::addRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue)
{
	m_VertexBuffers.AddVertex(left,bottom);
	m_VertexBuffers.AddVertex(right,top);
	m_VertexBuffers.AddVertex(left,top);

	m_VertexBuffers.AddVertex( left,bottom );
	m_VertexBuffers.AddVertex( right,bottom );
	m_VertexBuffers.AddVertex( right,top );
}

void Mesh::initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments,
	const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {

	destroyMesh();

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

			m_VertexBuffers.AddVertex(pos1[i]);
			m_VertexBuffers.AddVertex(pos2[i]);
			m_VertexBuffers.AddVertex(corners[i]);
		}
		pos1.clear();
		pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
	}

	initializeMesh(commandPool, graphicsQueue);
}


void Mesh::addVertex(const glm::vec2& pos, const glm::vec3& color)
{
	m_VertexBuffers.AddVertex(pos,color);
}


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
