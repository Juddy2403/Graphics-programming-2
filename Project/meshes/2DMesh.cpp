#include "2DMesh.h"
#include "vulkanbase/VulkanBase.h"

Mesh2D::Mesh2D() {
    //m_UBOMatrixes.proj[1][1] *= -1;
    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

}

Mesh2D::Mesh2D(std::vector<Vertex2D> &&vertices, std::vector<uint32_t> &&indices):Mesh2D() {
    m_Vertices = std::move(vertices);
    m_Indices = std::move(indices);
}

void Mesh2D::Update(uint32_t currentFrame) {

}

void Mesh2D::ClearVertices() {
    m_Vertices.clear();
}

void Mesh2D::ClearIndices() {
    m_Indices.clear();
}

void Mesh2D::UploadMesh(VkCommandPool const &commandPool, VkQueue const &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh2D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}

void Mesh2D::Destroy() {
    DestroyBuffers();
}

void Mesh2D::Draw(VkCommandBuffer const &commandBuffer, uint32_t currentFrame) const {
    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::GetPipelineLayout(), 0, 1,
                            &Level::Get2DDescriptorPool().GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh2D::DestroyBuffers() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}

void Mesh2D::AddVertex(const glm::vec2 &pos, const glm::vec3 &color) {
    AddVertex(Vertex2D{pos, color});
}

void Mesh2D::AddVertex(const Vertex2D &vertex) {
    if(m_UniqueVertices.count(vertex) == 0)
    {
        m_UniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
        m_Vertices.push_back(vertex);
    }
    m_Indices.push_back(m_UniqueVertices[vertex]);
}

