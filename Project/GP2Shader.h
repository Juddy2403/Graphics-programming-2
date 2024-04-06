#pragma once
#include <vulkan/vulkan_core.h>
#include <utility>
#include <vector>
#include <string>

class GP2Shader
{
public:
	GP2Shader(
		std::string  vertexShaderFile,
		std::string  fragmentShaderFile
	) : m_VertexShaderFile{std::move( vertexShaderFile )},
		m_FragmentShaderFile{std::move( fragmentShaderFile )}
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

    GP2Shader(const GP2Shader&) = delete;
    GP2Shader& operator=(const GP2Shader&) = delete;
    GP2Shader(const GP2Shader&&) = delete;
    GP2Shader& operator=(const GP2Shader&&) = delete;
private:

	VkShaderModule createShaderModule( const std::vector<char>& code);
	std::string m_VertexShaderFile;
	std::string m_FragmentShaderFile;

	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;

};
