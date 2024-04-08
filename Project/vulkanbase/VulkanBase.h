#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include "VulkanUtil.h"
#include "Shader.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <optional>
#include <set>
#include <limits>
#include <algorithm>
#include "CommandBuffer.h"
#include "CommandPool.h"
#include "3DMesh.h"
#include "Level.h"
#include "RenderPass.h"
#include <GraphicsPipeline.h>
#include "TimeManager.h"
#include <DepthBuffer.h>

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

class VulkanBase {
public:
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	static VkPhysicalDevice physicalDevice;
	static VkDevice device;
	static VkExtent2D swapChainExtent;

private:


	void initVulkan() {
		// week 06
		createInstance();
		setupDebugMessenger();
		createSurface();

		// week 05
		pickPhysicalDevice();
		createLogicalDevice();

		// week 04 
		createSwapChain();
		createImageViews();
		
		// week 03
		m_3DShader.Initialize();
        m_2DShader.Initialize();
		m_RenderPass.createRenderPass(swapChainImageFormat);
        Shader::CreateDescriptor();
        GraphicsPipeline::CreatePipelineLayout();
        m_3DGraphicsPipeline.createGraphicsPipeline(m_RenderPass.getRenderPass(), m_3DShader, Vertex3D::CreateVertexInputStateInfo());
        m_2DGraphicsPipeline.createGraphicsPipeline(m_RenderPass.getRenderPass(), m_2DShader, Vertex2D::CreateVertexInputStateInfo());
		m_RenderPass.createFrameBuffers(swapChainImageViews,swapChainExtent);
		// week 02
		//triangleMesh.AddVertex({-0.8f,0.4f}, {1.f,1.f,1.f} );
		//triangleMesh.UploadMesh(device, physicalDevice);
		//triangleMesh.InitializeCircle({0.f,0.f},0.3,50);
		//triangleMesh.InitializeRoundedRect(-0.3, 0.3, 0.3, -0.3,0.2,20);


		m_CommandPool = CommandPool{surface, findQueueFamilies(physicalDevice)};
		m_DepthBuffer.CreateDepthResources();
		m_CommandBuffer = CommandBuffer{ m_CommandPool.GetCommandPool() };
		m_Level.initializeLevel(m_CommandPool.GetCommandPool(), graphicsQueue);

		//createCommandPool();
		//createCommandBuffer();


		// week 06
		createSyncObjects();
	}

	void mainLoop() {
		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();
			// week 06
			TimeManager::GetInstance().Update();
			drawFrame();
		}
		vkDeviceWaitIdle(device);
	}

	void cleanup() {
		vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
		vkDestroyFence(device, inFlightFence, nullptr);

		m_CommandPool.DestroyCommandPool();
		/*for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(device, framebuffer, nullptr);
		}*/

		m_3DGraphicsPipeline.destroyGraphicsPipeline();
		m_2DGraphicsPipeline.destroyGraphicsPipeline();
        GraphicsPipeline::DestroyGraphicsPipelineLayout();
        Shader::DestroyDescriptorSetLayout();
		m_RenderPass.destroyRenderPass();

		//vkDestroyPipeline(device, m_3DGraphicsPipeline, nullptr);
		//vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		//vkDestroyRenderPass(device, renderPass, nullptr);

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(device, imageView, nullptr);
		}

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}
		vkDestroySwapchainKHR(device, swapChain, nullptr);

		//triangleMesh.destroyMesh();
		m_Level.destroyLevel();
		//m_Descriptor.DestroyUniformBuffers();
		vkDestroyDevice(device, nullptr);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void createSurface() {
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
			throw std::runtime_error("failed to create window surface!");
		}
	}

	Shader m_3DShader{"shaders/3Dshader.vert.spv",
                      "shaders/3Dshader.frag.spv" };
    Shader m_2DShader{"shaders/2Dshader.vert.spv",
                      "shaders/2Dshader.frag.spv" };
	CommandPool m_CommandPool{};
	CommandBuffer m_CommandBuffer{};
	Level m_Level{};
	RenderPass m_RenderPass{};
	GraphicsPipeline m_3DGraphicsPipeline{};
	GraphicsPipeline m_2DGraphicsPipeline{};
	DepthBuffer m_DepthBuffer;
	// Week 01: 
	// Actual window
	// simple fragment + vertex shader creation functions
	// These 5 functions should be refactored into a separate C++ class
	// with the correct internal state.

	GLFWwindow* window;
	void initWindow();

	//void drawScene();

	// Week 02
	// Queue families
	// CommandBuffer concept


	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice vkDevice);

	void drawFrame(uint32_t imageIndex);
	//void createCommandBuffer();
	//void createCommandPool(); 
	//void Buffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	
	// Week 03
	// Renderpass concept
	// Graphics pipeline
	
	//std::vector<VkFramebuffer> swapChainFramebuffers;
	/*VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;*/
	//VkRenderPass renderPass;

	//void createFrameBuffers();
	//void createRenderPass();
	//void createGraphicsPipeline();

	// Week 04
	// Swap chain and image view support

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;

	std::vector<VkImageView> swapChainImageViews;

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void createSwapChain();
	void createImageViews();

	// Week 05 
	// Logical and physical device

	VkQueue graphicsQueue;
	VkQueue presentQueue;
	
	void pickPhysicalDevice();
	bool isDeviceSuitable(VkPhysicalDevice device);
	void createLogicalDevice();

	// Week 06
	// Main initialization

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;

	VkSurfaceKHR surface;

	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkFence inFlightFence;

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	void setupDebugMessenger();
	std::vector<const char*> getRequiredExtensions();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	void createInstance();

	void createSyncObjects();
	void drawFrame();

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
		return VK_FALSE;
	}
};