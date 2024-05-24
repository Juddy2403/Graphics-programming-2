#pragma once
#include "meshes/3DMesh.h"
#include "meshes/2DMesh.h"
#include <memory>
#include <glm/gtc/matrix_transform.hpp>

class Mesh3D;
class Level
{
private:
	std::vector<std::unique_ptr<Mesh3D>> m_3DMeshes;
	std::vector<std::unique_ptr<Mesh2D>> m_2DMeshes;
    UniformBufferObject m_3DUBOMatrixes{};
    UniformBufferObject m_2DUBOMatrixes{};

    static DescriptorPool m_2DDescriptorPool;
public:
    static int m_AreNormalsEnabled;
    static DescriptorPool& Get2DDescriptorPool() { return m_2DDescriptorPool; }
	void Update(uint32_t currentFrame, const glm::mat4 &viewMatrix);
	void DestroyLevel();
	void Draw3DMeshes(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

    void Draw2DMeshes(VkCommandBuffer const &commandBuffer, uint32_t currentFrame) const;
    void WindowHasBeenResized(const glm::mat4& projMatrix);
    void InitializeLevel(const VkCommandPool &commandPool, const glm::mat4 &projMatrix);
};
