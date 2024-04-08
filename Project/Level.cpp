#include "Level.h"
#include "GraphicsPipeline.h"
#include "vulkanbase/VulkanBase.h"
#include <vulkanbase/VulkanUtil.h>
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>

DescriptorPool Level::m_2DDescriptorPool;

void Level::Update(uint32_t currentFrame, const glm::mat4 &viewMatrix) {
   m_3DUBOMatrixes.view = viewMatrix;
//        m_3DUBOMatrixes.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
//                                       glm::vec3(0.0f, 1.0f, 0.0f));
//    m_3DUBOMatrixes.proj = glm::perspective(glm::radians(45.0f),
//                                            VulkanBase::swapChainExtent.width /
//                                            (float) VulkanBase::swapChainExtent.height, 0.1f,
//                                            10.0f);
//    m_3DUBOMatrixes.proj[1][1] *= -1;
//    for (auto &mesh: m_3DMeshes) {
//        mesh->Update(currentFrame);
//    }
//    for (auto &mesh: m_2DMeshes) {
//        mesh->Update(currentFrame);
//    }
}

void Level::initializeLevel(const VkCommandPool &commandPool, const VkQueue &graphicsQueue, const glm::mat4& projMatrix) {
    //m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
//    std::vector<Vertex3D> vertices = {
//            Vertex3D{{-0.5f, -0.5f, -0.5f},
//                     {1.0f,  0.0f,  0.0f}},
//            Vertex3D{{0.5f, -0.5f, -0.5f},
//                     {0.0f, 1.0f,  0.0f}},
//            Vertex3D{{0.5f, 0.5f, -0.5f},
//                     {0.0f, 0.0f, 1.0f}},
//            Vertex3D{{-0.5f, 0.5f, -0.5f},
//                     {1.0f,  1.0f, 1.0f}}
//    };
//    m_3DUBOMatrixes.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f),
//                                       glm::vec3(0.0f, 1.0f, 0.0f));
//    m_3DUBOMatrixes.proj = glm::perspective(glm::radians(45.0f),
//                                            VulkanBase::swapChainExtent.width /
//                                            (float) VulkanBase::swapChainExtent.height, 0.1f,
//                                            10.0f);
//    m_3DUBOMatrixes.proj[1][1] *= -1;
    m_3DUBOMatrixes.proj = projMatrix;

    m_2DUBOMatrixes.model = glm::mat4(1.f);
    float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width)/ VulkanBase::swapChainExtent.height;
    glm::vec3 scaleFactors(1 / aspectRatio , 1  , 1.0f);
    m_2DUBOMatrixes.view = glm::scale(glm::mat4(1.0f), scaleFactors);
    m_2DUBOMatrixes.proj = glm::mat4(1.f);
    m_2DDescriptorPool.Initialize(Shader::GetDescriptorSetLayout());

    std::vector<Vertex2D> vertices = {
           {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
           {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
           {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
           {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
    };
    std::vector<uint16_t> indices = {0, 2, 1, 2, 0, 3};
    m_2DMeshes.emplace_back(std::make_unique<Mesh2D>(std::move(vertices), std::move(indices)));
    m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
//    vertices = {
//            Vertex3D{glm::vec3{-0.5f, -0.5f, 0.f}, glm::vec3{1.0f, 0.0f, 0.0f}},
//            Vertex3D{glm::vec3{0.5f, -0.5f, 0.f}, glm::vec3{0.0f, 1.0f, 0.0f}},
//            Vertex3D{glm::vec3{0.5f, 0.5f, 0.f}, glm::vec3{0.0f, 0.0f, 1.0f}},
//            Vertex3D{glm::vec3{-0.5f, 0.5f, 0.f}, glm::vec3{1.0f, 1.0f, 1.0f}}
//    };
//    indices = {0, 1, 2, 2, 3, 0};
//    m_3DMeshes.emplace_back(std::make_unique<Mesh3D>(std::move(vertices), std::move(indices)));
    m_3DMeshes[0]->InitializeCube({-0.25f, -0.25f, -0.25f}, 0.5);

    //m_3DMeshes[1]->InitializeCircle({0, 0}, 1, 30);
    //m_3DMeshes[1]->InitializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20);
    //m_3DMeshes[1]->InitializeRect(-0.3, 0.3, 0.3, -0.3);
    for (auto &mesh: m_3DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, graphicsQueue);
    }
    for (auto &mesh: m_2DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, graphicsQueue);
    }
}

void Level::destroyLevel() {
    for (auto &mesh: m_3DMeshes)
        mesh->Destroy();
    for (auto &mesh: m_2DMeshes) {
        mesh->Destroy();
    }
    m_2DDescriptorPool.DestroyUniformBuffers();
}

void Level::Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_3DMeshes) {
        mesh->draw(commandBuffer, currentFrame, m_3DUBOMatrixes);
    }
}
void Level::Draw2DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    m_2DDescriptorPool.UpdateUniformBuffer(currentFrame, m_2DUBOMatrixes);
    for (const auto &mesh: m_2DMeshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}


