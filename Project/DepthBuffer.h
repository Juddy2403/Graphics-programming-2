#pragma once

#include <vulkan/vulkan_core.h>
//TODO: not implemented yet
class DepthBuffer {
private:
    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;
public:
    void CreateDepthResources();
};
