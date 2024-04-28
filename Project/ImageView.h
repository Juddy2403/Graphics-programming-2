#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

struct ImageView final {
    std::vector<VkImage> m_SwapChainImages;
    VkFormat m_SwapChainImageFormat;
    std::vector<VkImageView> m_SwapChainImageViews;

    void CreateImageViews();
};

