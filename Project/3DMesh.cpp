#include "3DMesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/gtc/constants.hpp>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS

#include <glm/gtc/matrix_transform.hpp>

Mesh3D::Mesh3D(std::vector<Vertex3D> &&vertices, std::vector<uint16_t> &&indices): Mesh3D() {
    m_Vertices = std::move(vertices);
    m_Indices = std::move(indices);
}

Mesh3D::Mesh3D() {
    m_UBOMatrixes.model = glm::mat4(1.f);
    m_UBOMatrixes.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
                                     glm::vec3(0.0f, 1.0f, 0.0f));
    m_UBOMatrixes.proj = glm::perspective(glm::radians(45.0f),
                                          VulkanBase::swapChainExtent.width /
                                          (float) VulkanBase::swapChainExtent.height, 0.1f,
                                          10.0f);
    m_UBOMatrixes.proj[1][1] *= -1;
    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    m_DescriptorPool.Initialize(Shader::GetDescriptorSetLayout());
}

void Mesh3D::Update(uint32_t currentFrame) {
    float totalTime = TimeManager::GetInstance().GetTotalElapsed();

    m_UBOMatrixes.model = glm::rotate(glm::mat4(1.0f), totalTime * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

}

void Mesh3D::UploadMesh(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh3D::InitializeCircle(const glm::vec2 &center, float radius, int nrOfSegments) {
    m_Vertices.clear();
    m_Indices.clear();
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
}

void Mesh3D::InitializeRect(float top, float left, float bottom, float right) {
    m_Vertices.clear();
    m_Indices.clear();
    AddRect(top, left, bottom, right);

}

void Mesh3D::AddRect(float top, float left, float bottom, float right) {
    AddVertex(glm::vec3(left, bottom, 0));
    AddVertex(glm::vec3(right, top, 0));
    AddVertex(glm::vec3(left, top, 0));

    AddVertex(glm::vec3(left, bottom, 0));
    AddVertex(glm::vec3(right, bottom, 0));
    AddVertex(glm::vec3(right, top, 0));
}

void
Mesh3D::AddRectPlane(const glm::vec3 &bottomLeft, const glm::vec3 &topRight, bool isClockWise, bool areZValsInverted = false) {
    glm::vec3 topLeft(bottomLeft.x, topRight.y, (topRight.z < bottomLeft.z) ? topRight.z : bottomLeft.z);
    glm::vec3 bottomRight(topRight.x, bottomLeft.y, (topRight.z > bottomLeft.z) ? topRight.z : bottomLeft.z);
    if(areZValsInverted) {
        topLeft.z = (topRight.z > bottomLeft.z) ? topRight.z : bottomLeft.z;
        bottomRight.z = (topRight.z < bottomLeft.z) ? topRight.z : bottomLeft.z;
    }
    m_Vertices.emplace_back(bottomLeft);
    m_Vertices.emplace_back(bottomRight);
    m_Vertices.emplace_back(topLeft);
    m_Vertices.emplace_back(topRight);

    std::vector<uint16_t> indices;
    uint16_t indicesSize = m_Indices.empty() ? 0 : *std::max_element(m_Indices.begin(), m_Indices.end()) + 1;
    if (isClockWise) indices = {0, 2, 3, 0, 3, 1};
    else indices = {0, 3, 2, 0, 1, 3};
    for (auto &index: indices) {
        m_Indices.push_back(index + indicesSize);
    }

}

void Mesh3D::InitializeCube(const glm::vec3 &bottomLeftBackCorner, float sideLength) {
    m_Vertices.clear();
    m_Indices.clear();
    const glm::vec3 forward{ glm::vec3(0.0f, 0.0f, 1.0f) };
    const glm::vec3 up{ glm::vec3(0.0f, 1.0f, 0.0f) };
    const glm::vec3 right{ glm::vec3(1.0f, 0.0f, 0.0f) };

    const glm::vec3 bottomRightBackCorner = bottomLeftBackCorner + sideLength * right;
    const glm::vec3 topLeftBackCorner = bottomLeftBackCorner + sideLength * up;
    const glm::vec3 topRightBackCorner = bottomRightBackCorner + sideLength * up;
    const glm::vec3 bottomLeftFrontCorner = bottomLeftBackCorner + sideLength * forward;
    const glm::vec3 bottomRightFrontCorner = bottomRightBackCorner + sideLength * forward;
    const glm::vec3 topLeftFrontCorner = topLeftBackCorner + sideLength * forward;
    const glm::vec3 topRightFrontCorner = topRightBackCorner + sideLength * forward;

    AddRectPlane(bottomLeftBackCorner, topRightBackCorner, true); // back plane
    AddRectPlane(bottomLeftFrontCorner, topRightFrontCorner, false); // front plane
    AddRectPlane(bottomLeftBackCorner, topLeftFrontCorner, false); // left plane
    AddRectPlane(topRightBackCorner, bottomRightFrontCorner, false); // right plane
    AddRectPlane(topLeftBackCorner, topRightFrontCorner, true, true); // top plane
    AddRectPlane(bottomLeftBackCorner, bottomRightFrontCorner, false, true); // bottom plane
}

void Mesh3D::InitializeRoundedRect(float left, float top, float right, float bottom, float radius, int nrOfSegments) {

    m_Vertices.clear();
    m_Indices.clear();
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
    AddRect(pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x);
    AddRect(pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x);

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

}


void Mesh3D::AddVertex(const glm::vec3 &pos, const glm::vec3 &color) {
    m_Vertices.emplace_back(pos, color);
    m_Indices.emplace_back(static_cast<uint16_t>(m_Indices.size()));
}

void Mesh3D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}


void Mesh3D::draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, m_UBOMatrixes);
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






