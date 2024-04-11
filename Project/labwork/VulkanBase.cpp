#include "vulkanbase/VulkanBase.h"

VkPhysicalDevice VulkanBase::physicalDevice = VK_NULL_HANDLE;
VkDevice VulkanBase::device = VK_NULL_HANDLE;
VkExtent2D VulkanBase::swapChainExtent;

void VulkanBase::initWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    glfwSetWindowUserPointer(window, this);
    glfwSetKeyCallback(window, [](GLFWwindow* glfWwindow, int key, int scancode, int action, int mods) {
        void* pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
        vBase->keyEvent(key, scancode, action, mods);
    });
    glfwSetCursorPosCallback(window, [](GLFWwindow* glfWwindow, double xpos, double ypos) {
        void* pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
        vBase->mouseMove(glfWwindow, xpos, ypos);
    });
    glfwSetMouseButtonCallback(window, [](GLFWwindow* glfWwindow, int button, int action, int mods) {
        void* pUser = glfwGetWindowUserPointer(glfWwindow);
        VulkanBase* vBase = static_cast<VulkanBase*>(pUser);
        vBase->mouseEvent(glfWwindow, button, action, mods);
    });
}

void VulkanBase::keyEvent(int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS) {
        // Add the pressed key to the set
        m_PressedKeys.insert(key);
    } else if (action == GLFW_RELEASE) {
        // Remove the released key from the set
        m_PressedKeys.erase(key);
    }
}

void VulkanBase::ProcessInput(){

    if (m_PressedKeys.count(GLFW_KEY_LEFT_SHIFT)) m_Camera.m_MovementSpeed = 8.f;
    else m_Camera.m_MovementSpeed = 4.f;

    if (m_PressedKeys.count(GLFW_KEY_W))
        m_Camera.m_Origin += (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Forward;

    if (m_PressedKeys.count(GLFW_KEY_S))
        m_Camera.m_Origin -= (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Forward;

    if (m_PressedKeys.count(GLFW_KEY_A))
        m_Camera.m_Origin -= (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Right;

    if (m_PressedKeys.count(GLFW_KEY_D))
        m_Camera.m_Origin += (m_Camera.m_MovementSpeed * TimeManager::GetInstance().GetElapsed()) * m_Camera.m_Right;
}

void VulkanBase::mouseMove(GLFWwindow* window, double xpos, double ypos)
{
    int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    if (state == GLFW_PRESS)
    {
        float dx = m_LastMousePos.x - static_cast<float>(xpos);
        float dy = m_LastMousePos.y - static_cast<float>(ypos);

        if(dx != 0.f)
        m_Camera.m_TotalYaw -= m_Camera.m_RotationSpeed * TimeManager::GetInstance().GetElapsed() * dx;
        if(m_Camera.m_TotalYaw > 360.f)
            m_Camera.m_TotalYaw -= 360.f;
        if(m_Camera.m_TotalYaw < -360.f)
            m_Camera.m_TotalYaw += 360.f;
        if(dy != 0.f)
        m_Camera.m_TotalPitch += m_Camera.m_RotationSpeed * TimeManager::GetInstance().GetElapsed() * dy;
        m_Camera.m_TotalPitch = std::clamp(m_Camera.m_TotalPitch, -60.f, 60.f);
    }
    m_LastMousePos ={xpos, ypos};
}
void VulkanBase::mouseEvent(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
//        double xpos, ypos;
//        glfwGetCursorPos(window, &xpos, &ypos);

    }
}

void VulkanBase::drawFrame(uint32_t imageIndex) {
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_RenderPass.getRenderPass();
    renderPassInfo.framebuffer = m_RenderPass.getSwapChainFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;

    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

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
                      m_2DGraphicsPipeline.getGraphicsPipeline());

    m_Level.Draw2DMeshes(m_CommandBuffer.GetVkCommandBuffer(), imageIndex);

    vkCmdBindPipeline(m_CommandBuffer.GetVkCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
                      m_3DGraphicsPipeline.getGraphicsPipeline());

    m_Level.Draw3DMeshes(m_CommandBuffer.GetVkCommandBuffer(), imageIndex);

    vkCmdEndRenderPass(m_CommandBuffer.GetVkCommandBuffer());
}

QueueFamilyIndices VulkanBase::findQueueFamilies(VkPhysicalDevice vkDevice) {
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

        if (indices.isComplete()) {
            break;
        }

        i++;
    }

    return indices;
}


