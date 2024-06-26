#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#define GLFW_EXPOSE_NATIVE_WIN32

#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
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
#include "meshes/3DMesh.h"
#include "Level.h"
#include "RenderPass.h"
#include <GraphicsPipeline.h>
#include "TimeManager.h"
#include "Camera.h"
#include "SwapChain.h"
#include <unordered_set>

const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
};
enum class ShadingMode {
    lambertMode,
    normalMode,
    specularMode,
    combinedMode
};

class VulkanBase {
public:
    void run() {
        InitWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

    static VkPhysicalDevice physicalDevice;
    static VkDevice device;
    static VkExtent2D swapChainExtent;
    static VkQueue graphicsQueue;

private:
    void initVulkan() {
        // week 06
        CreateInstance();
        SetupDebugMessenger();
        CreateSurface();

        // week 05
        PickPhysicalDevice();
        CreateLogicalDevice();

        // week 04
        m_SwapChain.CreateSwapChain(surface, window, FindQueueFamilies(physicalDevice));
        m_SwapChain.GetImageView().CreateImageViews();

        // week 03
        m_3DShader.Initialize();
        m_2DShader.Initialize();
        m_RenderPass.CreateRenderPass(m_SwapChain.GetImageView().m_SwapChainImageFormat);
        Shader::CreateDescriptor();
        GraphicsPipeline::CreatePipelineLayout();
        m_3DGraphicsPipeline.CreateGraphicsPipeline(m_RenderPass.GetRenderPass(), m_3DShader,
                                                    Vertex3D::CreateVertexInputStateInfo());
        m_2DGraphicsPipeline.CreateGraphicsPipeline(m_RenderPass.GetRenderPass(), m_2DShader,
                                                    Vertex2D::CreateVertexInputStateInfo(), false);
        m_RenderPass.CreateFrameBuffers(m_SwapChain.GetImageView().m_SwapChainImageViews, swapChainExtent);
        m_Camera.Initialize(45.f, {0.f, 0.f, -2.f},
                            static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height));

        m_CommandPool = CommandPool{surface, FindQueueFamilies(physicalDevice)};
        m_CommandBuffer = CommandBuffer{m_CommandPool.GetCommandPool()};
        m_Level.InitializeLevel(m_CommandPool.GetCommandPool(), m_Camera.m_ProjectionMatrix);

        // week 06
        CreateSyncObjects();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ProcessInput();
            TimeManager::GetInstance().Update();
            if (m_HasWindowResized) {
                vkDeviceWaitIdle(device);
                m_SwapChain.DestroySwapChain();
                m_SwapChain.CreateSwapChain(surface, window, FindQueueFamilies(physicalDevice));
                m_SwapChain.GetImageView().CreateImageViews();
                m_RenderPass.DestroyRenderPass();
                m_RenderPass.CreateRenderPass(m_SwapChain.GetImageView().m_SwapChainImageFormat);
                m_Camera.CalculateProjectionMatrix(static_cast<float>(m_Width) / static_cast<float>(m_Height));
                m_Level.WindowHasBeenResized(m_Camera.m_ProjectionMatrix);
                m_RenderPass.CreateFrameBuffers(m_SwapChain.GetImageView().m_SwapChainImageViews, swapChainExtent);
                m_HasWindowResized = false;
            }
            m_Camera.Update();
            DrawFrame();

        }
        vkDeviceWaitIdle(device);
    }

    void cleanup() {
        vkDestroySemaphore(device, renderFinishedSemaphore, nullptr);
        vkDestroySemaphore(device, imageAvailableSemaphore, nullptr);
        vkDestroyFence(device, inFlightFence, nullptr);

        m_CommandPool.DestroyCommandPool();

        m_3DGraphicsPipeline.DestroyGraphicsPipeline();
        m_2DGraphicsPipeline.DestroyGraphicsPipeline();
        GraphicsPipeline::DestroyGraphicsPipelineLayout();
        Shader::DestroyDescriptorSetLayout();
        m_RenderPass.DestroyRenderPass();

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }
        m_SwapChain.DestroySwapChain();

        m_Level.DestroyLevel();
        vkDestroyDevice(device, nullptr);

        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void CreateSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    ShadingMode m_ShadingMode = ShadingMode::combinedMode;
    static uint32_t m_Width;
    static uint32_t m_Height;
    static bool m_HasWindowResized;
    Camera m_Camera{};
    Shader m_3DShader{"shaders/3Dshader.vert.spv",
                      "shaders/3Dshader.frag.spv"};
    Shader m_2DShader{"shaders/2Dshader.vert.spv",
                      "shaders/2Dshader.frag.spv"};
    CommandPool m_CommandPool{};
    CommandBuffer m_CommandBuffer{};
    Level m_Level{};
    RenderPass m_RenderPass{};
    GraphicsPipeline m_3DGraphicsPipeline{};
    GraphicsPipeline m_2DGraphicsPipeline{};
    glm::vec2 m_LastMousePos{};
    std::unordered_set<int> m_PressedKeys;
    GLFWwindow *window;

    void InitWindow();

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice vkDevice);

    void DrawFrame(uint32_t imageIndex);
    // Swap chain and image view support

    SwapChain m_SwapChain{};
    // Logical and physical device

    VkQueue presentQueue;

    void PickPhysicalDevice();

    bool IsDeviceSuitable(VkPhysicalDevice device);

    void CreateLogicalDevice();

    // Main initialization

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkSurfaceKHR surface;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo);

    void SetupDebugMessenger();

    static std::vector<const char *> GetRequiredExtensions();

    static bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

    void CreateInstance();

    void CreateSyncObjects();

    void DrawFrame();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                        void *pUserData) {
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    void KeyEvent(int key, int scancode, int action, int mods);

    void MouseMove(GLFWwindow *window, double xpos, double ypos);

    void MouseEvent(GLFWwindow *window, int button, int action, int mods);

    void ProcessInput();

    static void WindowResized(GLFWwindow *window, int width, int height);
};