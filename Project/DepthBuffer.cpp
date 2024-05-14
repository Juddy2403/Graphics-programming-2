#include "DepthBuffer.h"
#include <vulkanbase/VulkanUtil.h>
#include <vulkanbase/VulkanBase.h>

void DepthBuffer::CreateDepthResources() {
    VkFormat depthFormat = FindDepthFormat();

    ImageInfoStruct imageInfo {};
    imageInfo.width = VulkanBase::swapChainExtent.width;
    imageInfo.height = VulkanBase::swapChainExtent.height;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.properties = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    Texture::CreateImage(imageInfo, depthImage, depthImageMemory);

    depthImageView = ImageView::CreateImageView(depthImage, depthFormat,VK_IMAGE_ASPECT_DEPTH_BIT);
}

VkFormat DepthBuffer::FindSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling,
                                          VkFormatFeatureFlags features) {
    for (VkFormat format: candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(VulkanBase::physicalDevice, format, &props);
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }
    throw std::runtime_error("failed to find supported format!");
}

VkFormat DepthBuffer::FindDepthFormat() {
    return FindSupportedFormat(
            {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool DepthBuffer::HasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void DepthBuffer::DestroyDepthResources() {
    vkDestroyImageView(VulkanBase::device, depthImageView, nullptr);
    vkDestroyImage(VulkanBase::device, depthImage, nullptr);
    vkFreeMemory(VulkanBase::device, depthImageMemory, nullptr);
}
