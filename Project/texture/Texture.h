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
public:
    Texture() = default;
    Texture(const Texture& other) = default;
    Texture(Texture&& other) = default;
    Texture& operator=(const Texture& other) noexcept;
    Texture& operator=(Texture&& other) noexcept ;
    bool operator==(const Texture &rhs) const;
    bool operator!=(const Texture &rhs) const;
    ~Texture() = default;

    //TODO: Add a texture manager that checks whether a texture path is already loaded
    void CreateTextureImage(const VkCommandPool &commandPool, const std::string &path);
    VkImageView GetTextureImageView() { return m_TextureImageView;}
    void DestroyTexture();
    static void CreateImage(const ImageInfoStruct &imageInfo, VkImage &image, VkDeviceMemory &imageMemory);

    //---Texture Sampler---

    static void CreateTextureSampler();
    static VkSampler GetTextureSampler() { return m_TextureSampler;}
    static void DestroyTextureSampler();

private:
    VkImage m_TextureImage;
    VkDeviceMemory m_TextureImageMemory;
    VkImageView m_TextureImageView;
    static VkSampler m_TextureSampler;

    void CreateTextureImageView();
    static void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, const VkCommandPool& commandPool);
    static void CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, const VkCommandPool &commandPool);

};

