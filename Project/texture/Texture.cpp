#include "Texture.h"
#include "DataBuffer.h"
#include "vulkanbase/VulkanBase.h"

#define STB_IMAGE_IMPLEMENTATION

#include "lib/stb_image.h"
#include <stdexcept>

VkSampler Texture::m_TextureSampler;

void Texture::CreateTextureImage(const VkCommandPool &commandPool, const std::string &path) {
    int texWidth, texHeight, texChannels;
    stbi_uc *pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;

    if (!pixels) {
        throw std::runtime_error("failed to load texture image!");
    }

    //TODO: just use the data buffer class
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    DataBuffer::CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer,
                             stagingBufferMemory);

    void *data;
    vkMapMemory(VulkanBase::device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(VulkanBase::device, stagingBufferMemory);

    stbi_image_free(pixels);

    ImageInfoStruct imageInfo{};
    imageInfo.width = texWidth;
    imageInfo.height = texHeight;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    CreateImage(imageInfo, m_TextureImage, m_TextureImageMemory);

    TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                          VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool);
    CopyBufferToImage(stagingBuffer, m_TextureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight),
                      commandPool);
    TransitionImageLayout(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                          VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool);

    vkDestroyBuffer(VulkanBase::device, stagingBuffer, nullptr);
    vkFreeMemory(VulkanBase::device, stagingBufferMemory, nullptr);

    CreateTextureImageView();
}

void Texture::CreateImage(const ImageInfoStruct &imageInfo, VkImage &image, VkDeviceMemory &imageMemory) {

    VkImageCreateInfo vkImageCreateInfo{};
    vkImageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkImageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    vkImageCreateInfo.extent.width = imageInfo.width;
    vkImageCreateInfo.extent.height = imageInfo.height;
    vkImageCreateInfo.extent.depth = 1;
    vkImageCreateInfo.mipLevels = 1;
    vkImageCreateInfo.arrayLayers = 1;
    vkImageCreateInfo.format = imageInfo.format;
    vkImageCreateInfo.tiling = imageInfo.tiling;
    vkImageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    vkImageCreateInfo.usage = imageInfo.usage;
    vkImageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    vkImageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(VulkanBase::device, &vkImageCreateInfo, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(VulkanBase::device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = DataBuffer::FindMemoryType(memRequirements.memoryTypeBits, imageInfo.properties);

    if (vkAllocateMemory(VulkanBase::device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate image memory!");
    }

    vkBindImageMemory(VulkanBase::device, image, imageMemory, 0);
}

//TODO: All of the helper functions that submit commands so far have been set up to execute synchronously by waiting for
// the queue to become idle. For practical applications it is recommended to combine these operations in a single command
// buffer and execute them asynchronously for higher throughput, especially the transitions and copy in the createTextureImage function.
// Try to experiment with this by creating a setupCommandBuffer that the helper functions record commands into, and add a
// flushSetupCommands to execute the commands that have been recorded so far. It's best to do this after the texture mapping works
// to check if the texture resources are still set up correctly.


void Texture::TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout,
                                    const VkCommandPool &commandPool) {
    CommandBuffer commandBuffer{commandPool};
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.srcAccessMask = 0; // TODO
    barrier.dstAccessMask = 0; // TODO

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
               newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        throw std::invalid_argument("unsupported layout transition!");
    }

    vkCmdPipelineBarrier(
            commandBuffer.GetVkCommandBuffer(),
            sourceStage, destinationStage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
    );

    commandBuffer.EndRecording();
    commandBuffer.Submit();
    commandBuffer.FreeCommandBuffer(commandPool);
}

void Texture::CopyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,
                                const VkCommandPool &commandPool) {
    CommandBuffer commandBuffer{commandPool};
    commandBuffer.BeginRecording(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;

    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = {0, 0, 0};
    region.imageExtent = {
            width,
            height,
            1
    };

    vkCmdCopyBufferToImage(
            commandBuffer.GetVkCommandBuffer(),
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
    );

    commandBuffer.EndRecording();
    commandBuffer.Submit();
    commandBuffer.FreeCommandBuffer(commandPool);
}

void Texture::DestroyTexture() {
    vkDestroyImageView(VulkanBase::device, m_TextureImageView, nullptr);
    vkDestroyImage(VulkanBase::device, m_TextureImage, nullptr);
    vkFreeMemory(VulkanBase::device, m_TextureImageMemory, nullptr);
}

void Texture::CreateTextureImageView() {
    m_TextureImageView = ImageView::CreateImageView(m_TextureImage, VK_FORMAT_R8G8B8A8_SRGB);
}

void Texture::CreateTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;

    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(VulkanBase::physicalDevice, &properties);

    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    if (vkCreateSampler(VulkanBase::device, &samplerInfo, nullptr, &m_TextureSampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }

}

void Texture::DestroyTextureSampler() {
    vkDestroySampler(VulkanBase::device, m_TextureSampler, nullptr);
}

Texture &Texture::operator=(const Texture &other) noexcept {
    if (this != &other) {
        m_TextureImage = other.m_TextureImage;
        m_TextureImageMemory = other.m_TextureImageMemory;
        m_TextureImageView = other.m_TextureImageView;
    }
    return *this;
}

Texture &Texture::operator=(Texture &&other) noexcept {
    if (this != &other) {
        m_TextureImage = other.m_TextureImage;
        m_TextureImageMemory = other.m_TextureImageMemory;
        m_TextureImageView = other.m_TextureImageView;
        other.m_TextureImage = VK_NULL_HANDLE;
        other.m_TextureImageMemory = VK_NULL_HANDLE;
        other.m_TextureImageView = VK_NULL_HANDLE;
    }
    return *this;
}



