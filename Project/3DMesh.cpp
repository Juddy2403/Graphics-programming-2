#include "3DMesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS

Mesh3D::Mesh3D(std::vector<Vertex3D> &&vertices, std::vector<uint16_t> &&indices) : Mesh3D() {
    m_Vertices = std::move(vertices);
    m_Indices = std::move(indices);
}

Mesh3D::Mesh3D() {
    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_DescriptorPool.Initialize(Shader::GetDescriptorSetLayout());
}

void Mesh3D::Update(uint32_t currentFrame) {
    float totalTime = TimeManager::GetInstance().GetTotalElapsed();

    m_WorldMatrix = glm::rotate(glm::mat4(1.0f), totalTime * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Mesh3D::UploadMesh(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh3D::AddRectPlane(Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight, Vertex3D &bottomRight,
                          bool isClockWise, bool keepNormals = true) {

    if(!keepNormals)
    {
        glm::vec3 normal = glm::normalize(glm::cross(topLeft.m_Pos - bottomLeft.m_Pos, bottomRight.m_Pos - bottomLeft.m_Pos));
        if(!isClockWise) normal *= -1;
        bottomLeft.m_Normal = normal;
        topLeft.m_Normal = normal;
        bottomRight.m_Normal = normal;
        topRight.m_Normal = normal;
    }
    if (isClockWise) {
        AddVertex(bottomLeft);
        AddVertex(topLeft);
        AddVertex(topRight);
        AddVertex(bottomLeft);
        AddVertex(topRight);
        AddVertex(bottomRight);
        }
    else {
        AddVertex(bottomLeft);
        AddVertex(topRight);
        AddVertex(topLeft);
        AddVertex(bottomLeft);
        AddVertex(bottomRight);
        AddVertex(topRight);
        }

}

void Mesh3D::InitializeCube(const glm::vec3 &bottomLeftBackCorner, float sideLength) {
    m_Vertices.clear();
    m_Indices.clear();
    const glm::vec3 forward{glm::vec3(0.0f, 0.0f, 1.0f)};
    const glm::vec3 up{glm::vec3(0.0f, 1.0f, 0.0f)};
    const glm::vec3 right{glm::vec3(1.0f, 0.0f, 0.0f)};

    const glm::vec3 bottomRightBackCorner = bottomLeftBackCorner + sideLength * right;
    const glm::vec3 topLeftBackCorner = bottomLeftBackCorner + sideLength * up;
    const glm::vec3 topRightBackCorner = bottomRightBackCorner + sideLength * up;
    const glm::vec3 bottomLeftFrontCorner = bottomLeftBackCorner + sideLength * forward;
    const glm::vec3 bottomRightFrontCorner = bottomRightBackCorner + sideLength * forward;
    const glm::vec3 topLeftFrontCorner = topLeftBackCorner + sideLength * forward;
    const glm::vec3 topRightFrontCorner = topRightBackCorner + sideLength * forward;

    Vertex3D bottomLeftBackCornerVertex{bottomLeftBackCorner};
    Vertex3D topRightFrontCornerVertex{topRightFrontCorner};
    Vertex3D bottomRightBackCornerVertex{bottomRightBackCorner};
    Vertex3D topLeftFrontCornerVertex{topLeftFrontCorner};
    Vertex3D bottomLeftFrontCornerVertex{bottomLeftFrontCorner};
    Vertex3D topRightBackCornerVertex{topRightBackCorner};
    Vertex3D bottomRightFrontCornerVertex{bottomRightFrontCorner};
    Vertex3D topLeftBackCornerVertex{topLeftBackCorner};

    AddRectPlane(bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topRightBackCornerVertex, bottomRightBackCornerVertex, true, false); // back plane
    AddRectPlane(bottomLeftFrontCornerVertex, topLeftFrontCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, false, false); // front plane
    AddRectPlane(bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topLeftFrontCornerVertex, bottomLeftFrontCornerVertex, false, false); // left plane
    AddRectPlane(bottomRightBackCornerVertex, topRightBackCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, true, false); // right plane
    AddRectPlane(topLeftBackCornerVertex, topLeftFrontCornerVertex, topRightFrontCornerVertex, topRightBackCornerVertex,
                 true, false); // top plane
    AddRectPlane(bottomLeftBackCornerVertex, bottomLeftFrontCornerVertex,
                 bottomRightFrontCornerVertex, bottomRightBackCornerVertex, false, false); // bottom plane

}


void Mesh3D::AddVertex(const Vertex3D &vertex) {
    if(m_UniqueVertices.count(vertex) == 0)
    {
        m_UniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
        m_Vertices.push_back(vertex);
    }
    m_Indices.push_back(m_UniqueVertices[vertex]);
}

void Mesh3D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}


void Mesh3D::draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame, UniformBufferObject ubo) const {
    ubo.model = m_WorldMatrix;
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, ubo);
    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::m_PipelineLayout, 0, 1,
                            &m_DescriptorPool.GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t >(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh3D::ResetVertices(std::vector<Vertex3D> &&vertices) {
    m_Vertices.clear();
    m_Vertices = std::move(vertices);
}

void Mesh3D::ResetIndices(std::vector<uint16_t> &&indices) {
    m_Indices.clear();
    m_Indices = std::move(indices);
}

void Mesh3D::Destroy() {
    DestroyBuffers();
    m_DescriptorPool.DestroyUniformBuffers();
}

void Mesh3D::DestroyBuffers() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}






