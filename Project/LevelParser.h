#pragma once
class Mesh3D;
class Mesh2D;
namespace LevelParser{
    void ParseLevel(std::vector<std::unique_ptr<Mesh3D>> &m_3DMeshes,
                    std::vector<std::unique_ptr<Mesh2D>> &m_2DMeshes, const VkCommandPool &commandPool,
                    const std::string &filepath);
}
