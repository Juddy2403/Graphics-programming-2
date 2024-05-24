#include "TextureManager.h"

Texture TextureManager::m_DefaultTexture;
void TextureManager::LoadDefaultTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_DefaultTexture.CreateTextureImage(commandPool, path);
}

void TextureManager::UnloadDefaultTexture() {
    m_DefaultTexture.DestroyTexture();
}

void TextureManager::UploadAlbedoTexture(const VkCommandPool &commandPool, const std::string &path) {
    m_AlbedoTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetAlbedoImageView(m_AlbedoTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();

}

void TextureManager::UploadNormalTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_NormalTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetNormalImageView(m_NormalTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
    m_HasNormalMap = 1;
}

void TextureManager::UploadGlossTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_GlossTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetGlossImageView(m_GlossTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}

void TextureManager::UploadSpecularTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_SpecularTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetSpecularImageView(m_SpecularTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}

void TextureManager::DestroyTextures() {
    m_AlbedoTexture.DestroyTexture();
    m_NormalTexture.DestroyTexture();
    m_GlossTexture.DestroyTexture();
    m_SpecularTexture.DestroyTexture();
}
