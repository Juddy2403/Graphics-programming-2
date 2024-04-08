#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <unordered_map>
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
    std::unordered_map<Vertex3D, uint32_t> m_UniqueVertices{};
    std::vector<Vertex3D> m_Vertices = {};
    std::vector<uint16_t> m_Indices = {};
    UniformBufferObject m_UBOMatrixes{};
    DescriptorPool m_DescriptorPool;
    void DestroyBuffers();

    void AddVertex(const Vertex3D &vertex);
    void AddRectPlane(Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight, Vertex3D &bottomRight,
                      bool isClockWise, bool keepNormals);

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

    void Destroy();
	void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

};
