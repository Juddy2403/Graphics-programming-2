#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "DescriptorPool.h"
#include "DataBuffer.h"
#include "Vertex.h"
#include "Texture.h"

#include "Transform.h"
#include "TextureManager.h"

class DataBuffer;
class Mesh3D
{
private:
	std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::unordered_map<Vertex3D, uint32_t> m_UniqueVertices{};
    std::vector<Vertex3D> m_Vertices = {};
    std::vector<uint32_t> m_Indices = {};
    Transform m_Transform;
    TextureManager m_TextureManager;

    DescriptorPool m_DescriptorPool;
    void DestroyBuffers();
public:
    Mesh3D();
    explicit Mesh3D(std::vector<Vertex3D>&& vertices, std::vector<uint32_t>&& indices);
    Mesh3D(const Mesh3D& other) = delete;
    Mesh3D(Mesh3D&& other) = delete;
    Mesh3D& operator=(const Mesh3D& other) = delete;
    Mesh3D& operator=(Mesh3D&& other) = delete;
    ~Mesh3D() = default;

    Transform& GetTransform() { return m_Transform; }
    TextureManager& GetTextureManager() { return m_TextureManager; }
    void AddVertex(const Vertex3D &vertex);
	void Update(uint32_t currentFrame, UniformBufferObject ubo);
    void ClearVertices();
    void ClearIndices();
	void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
    void MapBuffers();

    void Destroy();
	void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

};
