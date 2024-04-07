#include "Mesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>

Mesh::Mesh(std::vector<Vertex> &&vertices, std::vector<uint16_t> &&indices){
    m_UBOMatrixes.model = glm::mat4(1.f);
    m_UBOMatrixes.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_UBOMatrixes.proj = glm::perspective(glm::radians(45.0f),
                                          VulkanBase::swapChainExtent.width / (float) VulkanBase::swapChainExtent.height, 0.1f,
                                          10.0f);
    m_UBOMatrixes.proj[1][1] *= -1;
    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_Vertices = std::move(vertices);
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(Vertex), m_Vertices.data());
    m_Indices = std::move(indices);
    m_IndexBuffer->Map(m_Indices.size() * sizeof(uint16_t), m_Indices.data());

    m_DescriptorPool.Initialize(Shader::GetDescriptorSetLayout());
}

void Mesh::Update(uint32_t currentFrame) {
    float totalTime = TimeManager::GetInstance().GetTotalElapsed();

    m_UBOMatrixes.model = glm::rotate(glm::mat4(1.0f), totalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

}

void Mesh::UploadMesh(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh::initializeCircle(const glm::vec2 &center, float radius, int nrOfSegments,
                            const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;

    glm::vec2 pos1 = {radius + center.x, center.y};

    for (int i = 1; i <= nrOfSegments; ++i) {
        const float angle = angleIncrement * i;
        glm::vec2 pos2 = {radius * cos(angle) + center.x, radius * sin(angle) + center.y};

        // Add vertex positions to the vertex buffer
        AddVertex({pos2, 0});
        AddVertex({center, 0});
        AddVertex({pos1, 0});
        pos1 = pos2;
    }

    UploadMesh(commandPool, graphicsQueue);
}

void Mesh::initializeRect(float top, float left, float bottom, float right, const VkCommandPool &commandPool,
                          const VkQueue &graphicsQueue) {
    addRect(top, left, bottom, right, commandPool, graphicsQueue);

    UploadMesh(commandPool, graphicsQueue);
}

void Mesh::addRect(float top, float left, float bottom, float right, const VkCommandPool &commandPool,
                   const VkQueue &graphicsQueue) {
    AddVertex(glm::vec3(left, bottom, 0));
    AddVertex(glm::vec3(right, top, 0));
    AddVertex(glm::vec3(left, top, 0));

    AddVertex(glm::vec3(left, bottom, 0));
    AddVertex(glm::vec3(right, bottom, 0));
    AddVertex(glm::vec3(right, top, 0));
}

void Mesh::initializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments,
                                 const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {


    // Calculate the corner vertices
    const std::vector<glm::vec2> corners = {
            {right, top},     // Top-right     0
            {left,  top},      // Top-left      1
            {left,  bottom},  // Bottom-left   2
            {right, bottom}     // Bottom-right  3
    };

    std::vector<glm::vec2> pos1;
    std::vector<glm::vec2> pos2;

    constexpr float rightAngle = glm::pi<float>() / 2.f;
    const float angleIncrement = rightAngle / nrOfSegments;

    for (size_t i = 0; i < 4; i++) {
        pos1.emplace_back(corners[i].x + radius * cos((angleIncrement * 0 + rightAngle * i)),
                          corners[i].y + radius * sin((angleIncrement * 0 + rightAngle * i)));
    }
    addRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x, commandPool, graphicsQueue);
    addRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x, commandPool, graphicsQueue);

    for (int j = 1; j <= nrOfSegments; ++j) {
        pos2.clear();

        for (size_t i = 0; i < 4; i++) {
            pos2.emplace_back(corners[i].x + radius * cos((angleIncrement * j + rightAngle * i)),
                              corners[i].y + radius * sin((angleIncrement * j + rightAngle * i)));

            AddVertex({pos1[i], 0});
            AddVertex({pos2[i], 0});
            AddVertex({corners[i], 0});
        }
        pos1.clear();
        pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
    }

    UploadMesh(commandPool, graphicsQueue);
}


void Mesh::AddVertex(const glm::vec3 &pos, const glm::vec3 &color) {
    m_Vertices.emplace_back(pos, color);
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_Indices.emplace_back(static_cast<uint16_t>(m_Indices.size()));
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}


void Mesh::draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, m_UBOMatrixes);
    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::m_PipelineLayout, 0, 1, &m_DescriptorPool.GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t >(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh::ResetVertices(std::vector<Vertex> &&vertices) {
    m_Vertices.clear();
    m_Vertices = std::move(vertices);
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(Vertex), m_Vertices.data());
}

void Mesh::ResetIndices(std::vector<uint16_t> &&indices) {
    m_Indices.clear();
    m_Indices = std::move(indices);
    m_IndexBuffer->Map(m_Indices.size() * sizeof(uint16_t), m_Indices.data());
}

void Mesh::Destroy() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
    m_DescriptorPool.DestroyUniformBuffers();
}


