#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "DepthBuffer.h"

class RenderPass
{
private:
	VkRenderPass m_RenderPass;
    static DepthBuffer m_DepthBuffer;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

public:
    static DepthBuffer& GetDepthBuffer() { return m_DepthBuffer; }
	void CreateFrameBuffers(const std::vector<VkImageView>& swapChainImageViews,
                            const VkExtent2D& swapChainExtent);
	void CreateRenderPass(const VkFormat& swapChainImageFormat);
	void DestroyRenderPass();
	VkRenderPass& GetRenderPass() { return m_RenderPass; }
	std::vector<VkFramebuffer>& GetSwapChainFramebuffers() { return m_SwapChainFramebuffers; }
};
