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

class Mesh2D {
private:
    std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::vector<Vertex2D> m_Vertices = {};
    std::vector<uint16_t> m_Indices = {};
    void DestroyBuffers();

    void AddRect(float top, float left, float bottom, float right);
    void AddVertex(const glm::vec2 &pos, const glm::vec3& color = {1, 1, 1});

public:
    Mesh2D();
    explicit Mesh2D(std::vector<Vertex2D>&& vertices, std::vector<uint16_t>&& indices);
    Mesh2D(const Mesh2D& other) = delete;
    Mesh2D(Mesh2D&& other) = delete;
    Mesh2D& operator=(const Mesh2D& other) = delete;
    Mesh2D& operator=(Mesh2D&& other) = delete;
    ~Mesh2D() = default;

    void Update(uint32_t currentFrame);
    void ResetVertices(std::vector<Vertex2D>&& vertices);
    void ResetIndices(std::vector<uint16_t>&& indices);
    void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
    void MapBuffers();

    void InitializeCircle(const glm::vec2 &center, float radius, int nrOfSegments);
    void InitializeRect(float top, float left, float bottom, float right);
    void InitializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments);

    void Destroy();
    void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;
};


