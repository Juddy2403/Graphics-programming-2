#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "GLFW/glfw3.h"
#include "CommandPool.h"
#include "ImageView.h"

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class SwapChain final {
private:
    VkSwapchainKHR m_SwapChain;
    ImageView m_ImageView;

    SwapChainSupportDetails QuerySwapChainSupport(const VkSurfaceKHR &surface);

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);

    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, GLFWwindow *window);

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);

public:
    void CreateSwapChain(const VkSurfaceKHR &surface, GLFWwindow *window, const QueueFamilyIndices &indices);
    VkSwapchainKHR &GetSwapChain() { return m_SwapChain; }
    ImageView &GetImageView() { return m_ImageView; }
    void DestroySwapChain();

};


