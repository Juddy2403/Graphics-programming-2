#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <unordered_map>
#include "DescriptorPool.h"
#include "DataBuffer.h"
#include "Vertex.h"
#include "../cmake-build-debug/_deps/glfw-src/deps/linmath.h"

class Mesh2D {
private:
    std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::unordered_map<Vertex2D, uint32_t> m_UniqueVertices{};
    std::vector<Vertex2D> m_Vertices = {};
    std::vector<uint16_t> m_Indices = {};
    void DestroyBuffers();

    void AddRect(float top, float left, float bottom, float right);
    void AddVertex(const glm::vec2 &pos, const glm::vec3& color = {1, 1, 1});
    void AddVertex(const Vertex2D &vertex);

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
    void InitializeRect(const glm::vec2 &bottomLeft, float sideLen);
    void InitializeRoundedRect(float left, float bottom, float right, float top, float radius, int nrOfSegments);

    void Destroy();
    void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;
};


