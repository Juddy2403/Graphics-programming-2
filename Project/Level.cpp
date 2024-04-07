#include "Level.h"
#include "GraphicsPipeline.h"
#include <vulkanbase/VulkanUtil.h>

void Level::Update(uint32_t currentFrame) {
    for (auto &mesh: m_Meshes) {
        mesh->Update(currentFrame);
    }
}

void Level::initializeLevel(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    //m_Meshes.emplace_back(std::make_unique<Mesh>());
    std::vector<Vertex> vertices = {
            Vertex{{-0.5f, -0.5f, -0.5f},
                   {1.0f,  0.0f,  0.0f}},
            Vertex{{0.5f, -0.5f, -0.5f},
                   {0.0f, 1.0f,  0.0f}},
            Vertex{{0.5f, 0.5f, -0.5f},
                   {0.0f, 0.0f, 1.0f}},
            Vertex{{-0.5f, 0.5f, -0.5f},
                   {1.0f,  1.0f, 1.0f}}
    };
    std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0};
    m_Meshes.emplace_back(std::make_unique<Mesh>(std::move(vertices), std::move(indices)));
    m_Meshes.emplace_back(std::make_unique<Mesh>());
//    vertices = {
//            Vertex{glm::vec3{-0.5f, -0.5f, 0.f}, glm::vec3{1.0f, 0.0f, 0.0f}},
//            Vertex{glm::vec3{0.5f, -0.5f, 0.f}, glm::vec3{0.0f, 1.0f, 0.0f}},
//            Vertex{glm::vec3{0.5f, 0.5f, 0.f}, glm::vec3{0.0f, 0.0f, 1.0f}},
//            Vertex{glm::vec3{-0.5f, 0.5f, 0.f}, glm::vec3{1.0f, 1.0f, 1.0f}}
//    };
//    indices = {0, 1, 2, 2, 3, 0};
//    m_Meshes.emplace_back(std::make_unique<Mesh>(std::move(vertices), std::move(indices)));
    m_Meshes[0]->InitializeCube({-0.25f, -0.25f, -0.25f}, 0.5);

    //m_Meshes[1]->InitializeCircle({0, 0}, 1, 30);
    //m_Meshes[1]->InitializeRoundedRect(-0.3, 0.3, 0.3, -0.3, 0.2, 20);
    //m_Meshes[1]->InitializeRect(-0.3, 0.3, 0.3, -0.3);
    for (auto &mesh: m_Meshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, graphicsQueue);
    }
}

void Level::destroyLevel() {
    for (auto &mesh: m_Meshes)
        mesh->Destroy();

}

void Level::DrawMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_Meshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}
