#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include "DepthBuffer.h"

class RenderPass
{
private:
	VkRenderPass m_RenderPass;
    DepthBuffer m_DepthBuffer;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

public:
	void createFrameBuffers(const std::vector<VkImageView>& swapChainImageViews,
		const VkExtent2D& swapChainExtent);
	void createRenderPass(const VkFormat& swapChainImageFormat);
	void destroyRenderPass();
	VkRenderPass& getRenderPass() { return m_RenderPass; }
	std::vector<VkFramebuffer>& getSwapChainFramebuffers() { return m_SwapChainFramebuffers; }
};
