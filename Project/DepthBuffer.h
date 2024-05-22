#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

class DepthBuffer {
private:
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    static VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    bool HasStencilComponent(VkFormat format);
public:
    VkImage GetDepthImage() { return depthImage; }
    VkImageView GetDepthImageView() { return depthImageView; }
    static VkFormat FindDepthFormat();
    void CreateDepthResources();
    void DestroyDepthResources();
};
