#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

struct ImageView final {
    std::vector<VkImage> m_SwapChainImages;
    VkFormat m_SwapChainImageFormat;
    std::vector<VkImageView> m_SwapChainImageViews;

    void CreateImageViews();

    static VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT);
};

