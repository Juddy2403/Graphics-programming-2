#include "TextureManager.h"

Texture TextureManager::LoadTexture(VkCommandPool const &commandPool, const std::string &path) {
    if (m_TextureMap.find(path) != m_TextureMap.end())
        return m_TextureMap[path];

    Texture texture{};
    texture.CreateTextureImage(commandPool, path);
    m_TextureMap[path] = texture;
    return texture;
}

void TextureManager::DestroyTextures() {
    for(auto& texture : m_TextureMap)
        texture.second.DestroyTexture();
    m_TextureMap.clear();
}
