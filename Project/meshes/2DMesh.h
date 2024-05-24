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

class Mesh2D {
private:
    std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::unordered_map<Vertex2D, uint32_t> m_UniqueVertices{};
    std::vector<Vertex2D> m_Vertices = {};
    std::vector<uint32_t> m_Indices = {};
    void DestroyBuffers();

public:
    Mesh2D();
    explicit Mesh2D(std::vector<Vertex2D>&& vertices, std::vector<uint32_t>&& indices);
    Mesh2D(const Mesh2D& other) = delete;
    Mesh2D(Mesh2D&& other) = delete;
    Mesh2D& operator=(const Mesh2D& other) = delete;
    Mesh2D& operator=(Mesh2D&& other) = delete;
    ~Mesh2D() = default;

    void Update(uint32_t currentFrame);
    void ClearVertices();
    void ClearIndices();
    void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
    void MapBuffers();

    void AddVertex(const glm::vec2 &pos, const glm::vec3& color = {1, 1, 1});
    void AddVertex(const Vertex2D &vertex);

    void Destroy();
    void Draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;
};


