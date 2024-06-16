#include "TextureLoader.h"
#include "TextureManager.h"

Texture TextureLoader::m_DefaultTexture;
void TextureLoader::LoadDefaultTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_DefaultTexture = TextureManager::GetInstance().LoadTexture(commandPool,path);
}

void TextureLoader::UploadAlbedoTexture(const VkCommandPool &commandPool, const std::string &path) {
    m_AlbedoTexture = TextureManager::GetInstance().LoadTexture(commandPool,path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetAlbedoImageView(m_AlbedoTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();

}

void TextureLoader::UploadNormalTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_NormalTexture = TextureManager::GetInstance().LoadTexture(commandPool,path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetNormalImageView(m_NormalTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
    m_HasNormalMap = 1;
}

void TextureLoader::UploadGlossTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_GlossTexture = TextureManager::GetInstance().LoadTexture(commandPool,path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetGlossImageView(m_GlossTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}

void TextureLoader::UploadSpecularTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_SpecularTexture = TextureManager::GetInstance().LoadTexture(commandPool,path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetSpecularImageView(m_SpecularTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}
