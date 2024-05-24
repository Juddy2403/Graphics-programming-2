#include "Level.h"
#include "vulkanbase/VulkanBase.h"
#include "MeshLoader.h"
#include <vulkanbase/VulkanUtil.h>

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

void Level::initializeLevel(const VkCommandPool &commandPool, const glm::mat4 &projMatrix) {
    Texture::CreateTextureSampler();
    TextureManager::LoadDefaultTexture(commandPool, "resources/textures/default.jpg");
    m_3DUBOMatrixes.proj = projMatrix;

    m_2DUBOMatrixes.model = glm::mat4(1.f);
    float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) / VulkanBase::swapChainExtent.height;
    glm::vec3 scaleFactors(1 / aspectRatio, 1, 1.0f);
    m_2DUBOMatrixes.view = glm::scale(glm::mat4(1.0f), scaleFactors);

    m_2DUBOMatrixes.proj = glm::mat4(1.f);
    m_2DDescriptorPool.Initialize();

    m_2DMeshes.emplace_back(std::make_unique<Mesh2D>());
    m_2DMeshes.back()->InitializeRect({-0.5f, -0.5f}, 0.5f);

    m_2DMeshes.emplace_back(std::make_unique<Mesh2D>());
    m_2DMeshes.back()->InitializeCircle({-0.5f, 0.5f}, 0.3f,20);

    m_2DMeshes.emplace_back(std::make_unique<Mesh2D>());
    m_2DMeshes.back()->InitializeRoundedRect(0.2f, 0.5f, 0.5f, 0.f, 0.1f, 20);

    m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
   // m_3DMeshes.back()->InitializeCube({-2.f, -2.f, 4.f}, 1);
    MeshLoader::InitializeCube(*m_3DMeshes.back(), {-2.f, -2.f, 4.f}, 1);

    m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
    MeshLoader::LoadModel(*m_3DMeshes.back(), "resources/vehicle.obj", true);
    m_3DMeshes.back()->GetTextureManager().UploadAlbedoTexture(commandPool, "resources/textures/vehicle_diffuse.png");
    m_3DMeshes.back()->GetTextureManager().UploadNormalTexture(commandPool, "resources/textures/vehicle_normal.png");
    m_3DMeshes.back()->GetTextureManager().UploadGlossTexture(commandPool, "resources/textures/vehicle_gloss.png");
    m_3DMeshes.back()->GetTextureManager().UploadSpecularTexture(commandPool, "resources/textures/vehicle_specular.png");
    m_3DMeshes.back()->GetTransform().Translate({0.f, -10.f, 50.f});
    m_3DMeshes.back()->GetTransform().SetRotationPerSecond({0.f, 90.f, 0.f});

    m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
    MeshLoader::InitializeSphere(*m_3DMeshes.back(),{3.f, 0.f, 5.f}, 1);

    for (auto &mesh: m_3DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, VulkanBase::graphicsQueue);
    }
    for (auto &mesh: m_2DMeshes) {
        mesh->MapBuffers();
        mesh->UploadMesh(commandPool, VulkanBase::graphicsQueue);
    }
}

void Level::destroyLevel() {
    for (auto &mesh: m_3DMeshes)
        mesh->Destroy();
    for (auto &mesh: m_2DMeshes) {
        mesh->Destroy();
    }
    m_2DDescriptorPool.DestroyUniformBuffers();
    TextureManager::UnloadDefaultTexture();
    Texture::DestroyTextureSampler();
}

void Level::Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    for (const auto &mesh: m_3DMeshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}

void Level::Draw2DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {
    m_2DDescriptorPool.UpdateUniformBuffer(currentFrame, m_2DUBOMatrixes);
    for (const auto &mesh: m_2DMeshes) {
        mesh->draw(commandBuffer, currentFrame);
    }
}

void Level::WindowHasBeenResized(const glm::mat4 &projMatrix) {
    m_3DUBOMatrixes.proj = projMatrix;
    float aspectRatio = static_cast<float>(VulkanBase::swapChainExtent.width) / VulkanBase::swapChainExtent.height;
    glm::vec3 scaleFactors(1 / aspectRatio, 1, 1.0f);
    m_2DUBOMatrixes.view = glm::scale(glm::mat4(1.0f), scaleFactors);
}


