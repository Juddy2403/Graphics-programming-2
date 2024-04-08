#include "Level.h"
#include "GraphicsPipeline.h"
#include <vulkanbase/VulkanUtil.h>

void Level::Update(uint32_t currentFrame) {
    for (auto &mesh: m_3DMeshes) {
        mesh->Update(currentFrame);
    }
    for (auto &mesh: m_2DMeshes) {
        mesh->Update(currentFrame);
    }
}

void Level::initializeLevel(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
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

}

void Level::Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_3DMeshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}
void Level::Draw2DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_2DMeshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}
