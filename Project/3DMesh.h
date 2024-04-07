#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DescriptorPool.h"
#include "DataBuffer.h"
#include "Vertex.h"

class DataBuffer;
class Mesh3D
{
private:
	std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::vector<Vertex3D> m_Vertices = {};
    std::vector<uint16_t> m_Indices = {};
    UniformBufferObject m_UBOMatrixes{};
    DescriptorPool m_DescriptorPool;
    void DestroyBuffers();

    void AddRect(float top, float left, float bottom, float right);
    void AddVertex(const glm::vec3& pos, const glm::vec3& color = {1,1,1});
    void AddRectPlane(const glm::vec3 &bottomLeft, const glm::vec3 &topRight, bool isClockWise, bool areZValsInverted);

public:
    Mesh3D();
    explicit Mesh3D(std::vector<Vertex3D>&& vertices, std::vector<uint16_t>&& indices);
    Mesh3D(const Mesh3D& other) = delete;
    Mesh3D(Mesh3D&& other) = delete;
    Mesh3D& operator=(const Mesh3D& other) = delete;
    Mesh3D& operator=(Mesh3D&& other) = delete;
    ~Mesh3D() = default;

	void Update(uint32_t currentFrame);
    void ResetVertices(std::vector<Vertex3D>&& vertices);
    void ResetIndices(std::vector<uint16_t>&& indices);
	void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
    void MapBuffers();

    void InitializeCube(const glm::vec3 &bottomLeftBackCorner, float sideLength);
	void InitializeCircle(const glm::vec2 &center, float radius, int nrOfSegments);
	void InitializeRect(float top, float left, float bottom, float right);
	void InitializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments);

    void Destroy();
	void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

};
