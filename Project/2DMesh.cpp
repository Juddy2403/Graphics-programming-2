
#include "2DMesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

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

void Mesh2D::ResetVertices(std::vector<Vertex2D> &&vertices) {
    m_Vertices.clear();
    m_Vertices = std::move(vertices);
}

void Mesh2D::ResetIndices(std::vector<uint32_t> &&indices) {
    m_Indices.clear();
    m_Indices = std::move(indices);
}

void Mesh2D::UploadMesh(VkCommandPool const &commandPool, VkQueue const &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh2D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}

void Mesh2D::InitializeCircle(const glm::vec2 &center, float radius, int nrOfSegments) {
    m_Vertices.clear();
    m_Indices.clear();
    const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;

    glm::vec2 pos1 = {radius + center.x, center.y};

    for (int i = 1; i <= nrOfSegments; ++i) {
        const float angle = angleIncrement * i;
        glm::vec2 pos2 = {radius * cos(angle) + center.x, radius * sin(angle) + center.y};

        // Add vertex positions to the vertex buffer
        AddVertex(pos2, {1.0f, 1.0f, 1.0f});
        AddVertex(pos1,{1.0f, 1.0f, 1.0f});
        AddVertex(center,{1.0f, 0.0f, 0.0f});

        pos1 = pos2;
    }
}

void Mesh2D::InitializeRect(const glm::vec2 &bottomLeft, float sideLen) {
    m_Vertices.clear();
    m_Indices.clear();
    AddRect(bottomLeft.y + sideLen, bottomLeft.x, bottomLeft.y, bottomLeft.x + sideLen);
}

void Mesh2D::InitializeRoundedRect(float left, float bottom, float right, float top, float radius, int nrOfSegments) {
    m_Vertices.clear();
    m_Indices.clear();
    // Calculate the corner vertices
    const std::vector<glm::vec2> corners = {
            {right, bottom},     // Top-right     0
            {left,  bottom},      // Top-left      1
            {left,  top},  // Bottom-left   2
            {right, top}     // Bottom-right  3
    };

    std::vector<glm::vec2> pos1;
    std::vector<glm::vec2> pos2;

    constexpr float rightAngle = glm::pi<float>() / 2.f;
    const float angleIncrement = rightAngle / nrOfSegments;

    for (size_t i = 0; i < 4; i++) {
        pos1.emplace_back(corners[i].x + radius * cos((angleIncrement * 0 + rightAngle * i)),
                          corners[i].y + radius * sin((angleIncrement * 0 + rightAngle * i)));
    }
    AddRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x);
    AddRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x);

    for (int j = 1; j <= nrOfSegments; ++j) {
        pos2.clear();

        for (size_t i = 0; i < 4; i++) {
            pos2.emplace_back(corners[i].x + radius * cos((angleIncrement * j + rightAngle * i)),
                              corners[i].y + radius * sin((angleIncrement * j + rightAngle * i)));

            AddVertex(pos1[i],{1.0f, 0.0f, 0.0f});
            AddVertex(corners[i],{1.0f, 0.0f, 0.0f});
            AddVertex(pos2[i],{1.0f, 0.0f, 0.0f});

        }
        pos1.clear();
        pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
    }
}

void Mesh2D::Destroy() {
    DestroyBuffers();
}

void Mesh2D::draw(VkCommandBuffer const &commandBuffer, uint32_t currentFrame) const {
    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::m_PipelineLayout, 0, 1,
                            &Level::Get2DDescriptorPool().GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh2D::DestroyBuffers() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}

void Mesh2D::AddRect(float top, float left, float bottom, float right) {
    AddVertex(glm::vec2(left, bottom),{1.0f, 0.0f, 0.0f});
    AddVertex(glm::vec2(left, top),{0.0f, 0.0f, 1.0f});
    AddVertex(glm::vec2(right, top),{0.0f, 1.0f, 0.0f});

    AddVertex(glm::vec2(left, bottom),{1.0f, 0.0f, 0.0f});
    AddVertex(glm::vec2(right, top),{1.0f, 1.0f, 1.0f});
    AddVertex(glm::vec2(right, bottom),{0.0f, 1.0f, 0.0f});

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

