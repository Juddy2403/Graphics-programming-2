#pragma once
#include <vulkan/vulkan_core.h>
#include <string>

struct ImageInfoStruct{
    uint32_t width;
    uint32_t height;
    VkFormat format;
    VkImageTiling tiling;
    VkImageUsageFlags usage;
    VkMemoryPropertyFlags properties;
};

class Texture final {
private:
    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;
    VkImageView m_TextureImageView;
    static VkSampler m_TextureSampler;
public:
    void CreateTextureImage(const VkCommandPool &commandPool, const std::string &path);
    void CreateTextureImageView();
    static void CreateTextureSampler();
    static VkSampler GetTextureSampler() { return m_TextureSampler;}
    VkImageView GetTextureImageView() { return m_TextureImageView;}
    void DestroyTexture();
    static void DestroyTextureSampler();
    static void CreateImage(const ImageInfoStruct &imageInfo, VkImage &image, VkDeviceMemory &imageMemory);
    static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, const VkCommandPool& commandPool);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, const VkCommandPool &commandPool);
};

