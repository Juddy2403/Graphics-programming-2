#include "Level.h"
#include "vulkanbase/VulkanBase.h"
#include "meshes/MeshLoader.h"
#include "LevelParser.h"
#include "texture/TextureManager.h"

DescriptorPool Level::m_2DDescriptorPool;
int Level::m_AreNormalsEnabled = 1;

void Level::Update(uint32_t currentFrame, const glm::mat4 &viewMatrix) {
    m_3DUBOMatrixes.view = viewMatrix;

    for (auto &mesh: m_3DMeshes) {
        mesh->Update(currentFrame, m_3DUBOMatrixes);
    }
    for (auto &mesh: m_2DMeshes) {
        mesh->Update(currentFrame);
    }
}

void Level::InitializeLevel(const VkCommandPool &commandPool, const glm::mat4 &projMatrix) {
    Texture::CreateTextureSampler();
    TextureLoader::LoadDefaultTexture(commandPool, "resources/textures/default.jpg");
    m_3DUBOMatrixes.proj = projMatrix;

    m_2DUBOMatrixes.model = glm::mat4(1.f);
    float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) / VulkanBase::swapChainExtent.height;
    glm::vec3 scaleFactors(1 / aspectRatio, 1, 1.0f);
    m_2DUBOMatrixes.view = glm::scale(glm::mat4(1.0f), scaleFactors);

    m_2DUBOMatrixes.proj = glm::mat4(1.f);
    m_2DDescriptorPool.Initialize();

    LevelParser::ParseLevel(m_3DMeshes, m_2DMeshes, commandPool, "resources/level.json");
    m_3DMeshes.shrink_to_fit();
    m_2DMeshes.shrink_to_fit();

    for (auto &mesh: m_3DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, VulkanBase::graphicsQueue);
    }
    for (auto &mesh: m_2DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, VulkanBase::graphicsQueue);
    }
}

void Level::DestroyLevel() {
    for (auto &mesh: m_3DMeshes)
        mesh->Destroy();
    for (auto &mesh: m_2DMeshes) {
        mesh->Destroy();
    }
    m_2DDescriptorPool.DestroyUniformBuffers();
    TextureManager::GetInstance().DestroyTextures();
    Texture::DestroyTextureSampler();
}

void Level::Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_3DMeshes) {
        mesh->Draw(commandBuffer, currentFrame);
    }
}

void Level::Draw2DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    m_2DDescriptorPool.UpdateUniformBuffer(currentFrame, m_2DUBOMatrixes);
    for (const auto &mesh: m_2DMeshes) {
        mesh->Draw(commandBuffer, currentFrame);
    }
}

void Level::WindowHasBeenResized(const glm::mat4 &projMatrix) {
    m_3DUBOMatrixes.proj = projMatrix;
    float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) / VulkanBase::swapChainExtent.height;
    glm::vec3 scaleFactors(1 / aspectRatio, 1, 1.0f);
    m_2DUBOMatrixes.view = glm::scale(glm::mat4(1.0f), scaleFactors);
}


