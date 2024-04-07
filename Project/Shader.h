#pragma once

#include <vulkan/vulkan_core.h>
#include <utility>
#include <vector>
#include <string>
#include "DataBuffer.h"
#include "Vertex.h"
#include <memory>
#include "DescriptorPool.h"

class Shader {
public:
    Shader(
            std::string vertexShaderFile,
            std::string fragmentShaderFile
    ) : m_VertexShaderFile{std::move(vertexShaderFile)},
        m_FragmentShaderFile{std::move(fragmentShaderFile)} {}

    ~Shader() = default;

    std::vector<VkPipelineShaderStageCreateInfo> &GetShaderStages() { return m_ShaderStages; }

    void Initialize();

    void DestroyShaderModules();

    static VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateInfo();

    VkPipelineShaderStageCreateInfo CreateFragmentShaderInfo();

    VkPipelineShaderStageCreateInfo CreateVertexShaderInfo();

    Shader(const Shader &) = delete;

    Shader &operator=(const Shader &) = delete;

    Shader(const Shader &&) = delete;

    Shader &operator=(const Shader &&) = delete;

    static void CreateDescriptor();

    static const VkDescriptorSetLayout &GetDescriptorSetLayout() { return m_DescriptorSetLayout; }

    static void DestroyDescriptorSetLayout();

private:

    static VkShaderModule CreateShaderModule(const std::vector<char> &code);

    std::string m_VertexShaderFile;
    std::string m_FragmentShaderFile;

    std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

    static VkDescriptorSetLayout m_DescriptorSetLayout;
    UniformBufferObject m_UBOSrc{};

};
