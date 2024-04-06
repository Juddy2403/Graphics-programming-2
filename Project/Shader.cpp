#include "Shader.h"
#include <vulkanbase/VulkanUtil.h>
#include <vulkanbase/VulkanBase.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include <glm/gtc/matrix_transform.hpp>

VkDescriptorSetLayout Shader::m_DescriptorSetLayout;

void Shader::Initialize()
{
    m_DescriptorPool = std::make_unique<DescriptorPool>();

	m_ShaderStages.push_back(CreateVertexShaderInfo());
	m_ShaderStages.push_back(CreateFragmentShaderInfo());


}

void Shader::UpdateUniformBuffer(uint32_t currentFrame,const VkCommandPool& commandPool, const VkQueue& graphicsQueue) {
    float totalTime = TimeManager::GetInstance().GetTotalElapsed();

    m_UBOSrc.model = glm::rotate(glm::mat4(1.0f), totalTime * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_UBOSrc.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    m_UBOSrc.proj = glm::perspective(glm::radians(45.0f),
                                VulkanBase::swapChainExtent.width / (float) VulkanBase::swapChainExtent.height, 0.1f,
                                10.0f);
    m_UBOSrc.proj[1][1] *= -1;

    m_DescriptorPool->UpdateUniformBuffer(currentFrame, m_UBOSrc);
}

void Shader::DestroyShaderModules()
{
	for (VkPipelineShaderStageCreateInfo& stageInfo : m_ShaderStages)
	{
		vkDestroyShaderModule(VulkanBase::device, stageInfo.module, nullptr);
	}
	m_ShaderStages.clear();
}

VkPipelineShaderStageCreateInfo Shader::CreateFragmentShaderInfo() {
	std::vector<char> fragShaderCode = readFile(m_FragmentShaderFile);
	VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

VkPipelineShaderStageCreateInfo Shader::CreateVertexShaderInfo() {
	std::vector<char> vertShaderCode = readFile(m_VertexShaderFile);
	VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";
	return vertShaderStageInfo;
}

VkPipelineInputAssemblyStateCreateInfo Shader::CreateInputAssemblyStateInfo()
{
	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;
	return inputAssembly;
}

VkShaderModule Shader::CreateShaderModule(const std::vector<char>& code) {
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(VulkanBase::device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

void Shader::CreateDescriptor() {
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboLayoutBinding.pImmutableSamplers = nullptr; // Optional

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(VulkanBase::device, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }

    m_DescriptorPool->Initialize(m_DescriptorSetLayout);
}

void Shader::BindDescriptorSet(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout, size_t index) {
    m_DescriptorPool->BindDescriptorSet(commandBuffer, pipelineLayout, index);
}

void Shader::DestroyDescriptorSetLayout() {
    vkDestroyDescriptorSetLayout(VulkanBase::device, m_DescriptorSetLayout, nullptr);
}

void Shader::DestroyDescriptorBuffers() {
    m_DescriptorPool->DestroyUniformBuffers();
}
