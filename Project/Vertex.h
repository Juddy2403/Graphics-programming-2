#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <array>

//to fix the alignment requirements most of the time
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#include <glm/glm.hpp>

struct Vertex {
    //should be a vec3
    alignas(16) glm::vec3 m_Pos{};
    //glm::vec2 m_TexCoord;
    //glm::vec3 m_Normal;
    //glm::vec3 m_Tangent;
    alignas(16) glm::vec3 m_Color{ 1,1,1 };

    explicit Vertex(const glm::vec3 &pos,const glm::vec3 &color = {1,1,1}) : m_Pos(pos), m_Color(color) {}

    static VkPipelineVertexInputStateCreateInfo CreateVertexInputStateInfo()
    {
        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

        static auto bindingDescription = Vertex::getBindingDescription();
        static auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        //vertexInputInfo.vertexBindingDescriptionCount = 0; //&m_VertexInputBindingDescription
        //vertexInputInfo.vertexAttributeDescriptionCount = 0;

        return vertexInputInfo;
    }

    static VkVertexInputBindingDescription getBindingDescription() {

        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions{};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, m_Pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 2;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, m_Color);

        return attributeDescriptions;
    }
};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
};
