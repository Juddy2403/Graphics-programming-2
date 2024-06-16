#include "3DMesh.h"
#include "vulkanbase/VulkanBase.h"

Mesh3D::Mesh3D(std::vector<Vertex3D> &&vertices, std::vector<uint32_t> &&indices) : Mesh3D() {
    m_Vertices = std::move(vertices);
    m_Indices = std::move(indices);
}

Mesh3D::Mesh3D(): m_TextureManager(m_DescriptorPool) {
    m_DescriptorPool.Initialize();
    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void Mesh3D::Update(uint32_t currentFrame, UniformBufferObject ubo) {
    m_Transform.Update();
    ubo.model = m_Transform.GetWorldMatrix();
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, ubo);
}

void Mesh3D::UploadMesh(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh3D::AddVertex(const Vertex3D &vertex) {
    if (m_UniqueVertices.count(vertex) == 0) {
        m_UniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
        m_Vertices.push_back(vertex);
    }
    m_Indices.push_back(m_UniqueVertices[vertex]);
}

void Mesh3D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}

void Mesh3D::Draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {

    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    if(Level::m_AreNormalsEnabled == 1)
        vkCmdPushConstants(commandBuffer, GraphicsPipeline::GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT,
                           sizeof(glm::vec3), sizeof(int), &m_TextureManager.m_HasNormalMap);
    else
        vkCmdPushConstants(commandBuffer, GraphicsPipeline::GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT,
                           sizeof(glm::vec3), sizeof(int), &Level::m_AreNormalsEnabled);
    vkCmdPushConstants(commandBuffer, GraphicsPipeline::GetPipelineLayout(), VK_SHADER_STAGE_FRAGMENT_BIT,
                       sizeof(glm::vec3)+ sizeof(int)*2, sizeof(int), &m_DoesHavePBRMaterial);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::GetPipelineLayout(), 0, 1,
                            &m_DescriptorPool.GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t >(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh3D::ClearVertices() {
    m_Vertices.clear();
}

void Mesh3D::ClearIndices() {
    m_Indices.clear();
}

void Mesh3D::Destroy() {
    DestroyBuffers();
    m_DescriptorPool.DestroyUniformBuffers();
}

void Mesh3D::DestroyBuffers() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}









