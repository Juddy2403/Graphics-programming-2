#include "vulkanbase/VulkanBase.h"

VkPhysicalDevice VulkanBase::physicalDevice = VK_NULL_HANDLE;
VkDevice VulkanBase::device = VK_NULL_HANDLE;
VkExtent2D VulkanBase::swapChainExtent;
uint32_t VulkanBase::m_Width = 800;
uint32_t VulkanBase::m_Height = 600;
VkQueue VulkanBase::graphicsQueue;
bool VulkanBase::m_HasWindowResized = false;

void VulkanBase::WindowResized(GLFWwindow *window, int width, int height) {
    m_Width = width;
    m_Height = height;
    m_HasWindowResized = true;
}

void VulkanBase::InitWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    window = glfwCreateWindow(m_Width, m_Height, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow *glfWwindow, int key, int scancode, int action, int mods) {
        void *pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase *vBase = static_cast<VulkanBase *>(pUser);
        vBase->KeyEvent(key, scancode, action, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow *glfWwindow, double xpos, double ypos) {
        void *pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase *vBase = static_cast<VulkanBase *>(pUser);
        vBase->MouseMove(glfWwindow, xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow *glfWwindow, int button, int action, int mods) {
        void *pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase *vBase = static_cast<VulkanBase *>(pUser);
        vBase->MouseEvent(glfWwindow, button, action, mods);
    });

    glfwSetFramebufferSizeCallback(window, VulkanBase::WindowResized);
}

void VulkanBase::KeyEvent(int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Add the pressed key to the set
        m_PressedKeys.insert(key);
    } else if (action == GLFW_RELEASE) {
        // Remove the released key from the set
        m_PressedKeys.erase(key);
    }

    if (key == GLFW_KEY_N && action == GLFW_PRESS) {
        Level::m_AreNormalsEnabled = Level::m_AreNormalsEnabled == 1 ? 0 : 1;
    }

    if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
        m_ShadingMode = static_cast<ShadingMode>((static_cast<int>(m_ShadingMode) + 1) % 4);
    }

    if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
        m_ShadingMode = static_cast<ShadingMode>((static_cast<int>(m_ShadingMode) - 1));
        if(static_cast<int>(m_ShadingMode) < 0)
            m_ShadingMode = static_cast<ShadingMode>(3);
    }
}

void VulkanBase::ProcessInput() {

    if (m_PressedKeys.count(GLFW_KEY_LEFT_SHIFT)) m_Camera.m_MovementSpeed = 15.f;
    else m_Camera.m_MovementSpeed = 10.f;

    if (m_PressedKeys.count(GLFW_KEY_W))
        m_Camera.m_Origin += (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Forward;

    if (m_PressedKeys.count(GLFW_KEY_S))
        m_Camera.m_Origin -= (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Forward;

    if (m_PressedKeys.count(GLFW_KEY_A))
        m_Camera.m_Origin -= (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Right;

    if (m_PressedKeys.count(GLFW_KEY_D))
        m_Camera.m_Origin += (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Right;

}

void VulkanBase::MouseMove(GLFWwindow *window, double xpos, double ypos) {
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS) {
        float dx = m_LastMousePos.x - static_cast<float>(xpos);
        float dy = m_LastMousePos.y - static_cast<float>(ypos);

        if (dx != 0.f)
            m_Camera.m_TotalYaw -= m_Camera.m_RotationSpeed * TimeManager::GetInstance().GetElapsed() * dx;
        if (m_Camera.m_TotalYaw > 360.f)
            m_Camera.m_TotalYaw -= 360.f;
        if (m_Camera.m_TotalYaw < -360.f)
            m_Camera.m_TotalYaw += 360.f;
        if (dy != 0.f)
            m_Camera.m_TotalPitch -= m_Camera.m_RotationSpeed * TimeManager::GetInstance().GetElapsed() * dy;
        m_Camera.m_TotalPitch = std::clamp(m_Camera.m_TotalPitch, -60.f, 60.f);
    }
    m_LastMousePos = {xpos, ypos};
}

void VulkanBase::MouseEvent(GLFWwindow *window, int button, int action, int mods) {
//    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
////        double xpos, ypos;
////        glfwGetCursorPos(window, &xpos, &ypos);
//
//    }
}

void VulkanBase::DrawFrame(uint32_t imageIndex) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass.GetRenderPass();
    renderPassInfo.framebuffer = m_RenderPass.GetSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.2f, 0.4f, 0.6f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(m_CommandBuffer.GetVkCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapChainExtent.width;
    viewport.height = (float) swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(m_CommandBuffer.GetVkCommandBuffer(), 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapChainExtent;
    vkCmdSetScissor(m_CommandBuffer.GetVkCommandBuffer(), 0, 1, &scissor);

    vkCmdBindPipeline(m_CommandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_2DGraphicsPipeline.GetGraphicsPipeline());

    m_Level.Draw2DMeshes(m_CommandBuffer.GetVkCommandBuffer(), imageIndex);

    vkCmdBindPipeline(m_CommandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_3DGraphicsPipeline.GetGraphicsPipeline());

    m_Level.Draw3DMeshes(m_CommandBuffer.GetVkCommandBuffer(), imageIndex);

    vkCmdEndRenderPass(m_CommandBuffer.GetVkCommandBuffer());
}

QueueFamilyIndices VulkanBase::FindQueueFamilies(VkPhysicalDevice vkDevice) {
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(vkDevice, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto &queueFamily: queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }

        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(vkDevice, i, surface, &presentSupport);

        if (presentSupport) {
            indices.presentFamily = i;
        }

        if (indices.IsComplete()) {
            break;
        }

        i++;
    }

    return indices;
}

void VulkanBase::PickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    std::vector<VkPhysicalDevice> devices{deviceCount};
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }

    for (const auto &device: devices) {
        if (IsDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if (physicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

bool VulkanBase::IsDeviceSuitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = FindQueueFamilies(device);
    bool extensionsSupported = CheckDeviceExtensionSupport(device);

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

    return indices.IsComplete() && extensionsSupported && supportedFeatures.samplerAnisotropy;
}

void VulkanBase::CreateLogicalDevice() {
    QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily: uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;

    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
}

void VulkanBase::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT &createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}

void VulkanBase::SetupDebugMessenger() {
    if (!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void VulkanBase::CreateSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS ||
        vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(device, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
    }

}

void VulkanBase::DrawFrame() {
    vkWaitForFences(device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(device, 1, &inFlightFence);

    uint32_t imageIndex;
    vkAcquireNextImageKHR(device, m_SwapChain.GetSwapChain(), UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE,
                          &imageIndex);

    m_CommandBuffer.Reset();
    m_CommandBuffer.BeginRecording();
    vkCmdPushConstants(m_CommandBuffer.GetVkCommandBuffer(), GraphicsPipeline::GetPipelineLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(glm::vec3), &m_Camera.m_Origin);
    vkCmdPushConstants(m_CommandBuffer.GetVkCommandBuffer(), GraphicsPipeline::GetPipelineLayout(),
                       VK_SHADER_STAGE_FRAGMENT_BIT,
                       sizeof(glm::vec3) + sizeof(int), sizeof(int), &m_ShadingMode);
    m_Level.Update(imageIndex, m_Camera.m_ViewMatrix);

    DrawFrame(imageIndex);
    m_CommandBuffer.EndRecording();

    VkSubmitInfo submitInfo{};
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    m_CommandBuffer.Submit(submitInfo, inFlightFence);

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_SwapChain.GetSwapChain()};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;

    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);
}

bool CheckValidationLayerSupport() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char *layerName: validationLayers) {
        bool layerFound = false;

        for (const auto &layerProperties: availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char *> VulkanBase::GetRequiredExtensions() {
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char *> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

bool VulkanBase::CheckDeviceExtensionSupport(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto &extension: availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

void VulkanBase::CreateInstance() {
    if (enableValidationLayers && !CheckValidationLayerSupport()) {
        throw std::runtime_error("validation layers requested, but not available!");
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Hello Triangle";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = GetRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (enableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        PopulateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }
}
