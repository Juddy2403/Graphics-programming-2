#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <memory>

#include "Descriptor.h"
#include "DataBuffer.h"
#include "Vertex.h"

class DataBuffer;
class Mesh
{
private:
	std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::vector<Vertex> m_Vertices = {
            Vertex{glm::vec3{-0.5f, -0.5f,0.f}, glm::vec3{1.0f, 0.0f, 0.0f}},
            Vertex{glm::vec3{0.5f, -0.5f,0.f},  glm::vec3{0.0f, 1.0f, 0.0f}},
            Vertex{glm::vec3{0.5f, 0.5f,0.f},   glm::vec3{0.0f, 0.0f, 1.0f}},
            Vertex{glm::vec3{-0.5f, 0.5f,0.f},  glm::vec3{1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint16_t> m_Indices = { 0, 1, 2, 2, 3, 0 };

public:
    explicit Mesh(std::vector<Vertex>&& vertices, std::vector<uint16_t>&& indices);
    Mesh(const Mesh& other) = delete;
    Mesh(Mesh&& other) = delete;
    Mesh& operator=(const Mesh& other) = delete;
    Mesh& operator=(Mesh&& other) = delete;
    ~Mesh() = default;

	void Update(uint32_t currentFrame);
    void ResetVertices(std::vector<Vertex>&& vertices);
    void ResetIndices(std::vector<uint16_t>&& indices);
	void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeCircle(const glm::vec2& center, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void addRect(float top, float left, float bottom, float right, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments, const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
	void AddVertex(const glm::vec3& pos, const glm::vec3& color = {1,1,1});

    void Destroy();
	void draw(const VkCommandBuffer& commandBuffer, uint32_t currentFrame, const Descriptor& descriptor) const;
};
