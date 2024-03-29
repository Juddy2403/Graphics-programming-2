#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>

class GP2Shader
{
public:
	GP2Shader(
		const std::string& vertexShaderFile,
		const std::string& fragmentShaderFile
	) : m_VertexShaderFile{ vertexShaderFile },
		m_FragmentShaderFile{ fragmentShaderFile }
	{}
	~GP2Shader() = default;
	std::vector<VkPipelineShaderStageCreateInfo>& getShaderStages()
	{
		return m_ShaderStages;
	}

	void Initialize();
	void DestroyShaderModules();
	VkPipelineInputAssemblyStateCreateInfo createInputAssemblyStateInfo();
	VkPipelineShaderStageCreateInfo createFragmentShaderInfo();
	VkPipelineShaderStageCreateInfo createVertexShaderInfo();

private:

	VkShaderModule createShaderModule( const std::vector<char>& code);
	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

	GP2Shader(const GP2Shader&) = delete;
	GP2Shader& operator=(const GP2Shader&) = delete;
	GP2Shader(const GP2Shader&&) = delete;
	GP2Shader& operator=(const GP2Shader&&) = delete;
};
