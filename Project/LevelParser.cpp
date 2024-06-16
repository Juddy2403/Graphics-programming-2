#include <fstream>
#include "lib/json.hpp"
#include "meshes/3DMesh.h"
#include "meshes/2DMesh.h"
#include "meshes/MeshLoader.h"
#include "LevelParser.h"

void LevelParser::ParseLevel(std::vector<std::unique_ptr<Mesh3D>> &m_3DMeshes,
                             std::vector<std::unique_ptr<Mesh2D>> &m_2DMeshes, const VkCommandPool &commandPool,
                             const std::string &filepath) {
    std::ifstream file(filepath);
    nlohmann::json levelData;
    file >> levelData;

    for (const auto &meshData: levelData["meshes"]) {
        if (meshData["type"] == "2D") {
            m_2DMeshes.emplace_back(std::make_unique<Mesh2D>());
            auto &mesh = *m_2DMeshes.back();
            if (meshData["shape"] == "rect") {
                glm::vec2 position = {meshData["position"][0], meshData["position"][1]};
                MeshLoader::InitializeRect(mesh, position, meshData["size"]);
            } else if (meshData["shape"] == "circle") {
                glm::vec2 position = {meshData["position"][0], meshData["position"][1]};
                MeshLoader::InitializeCircle(mesh, position, meshData["radius"], meshData["segments"]);
            } else if (meshData["shape"] == "roundedRect") {
                const glm::vec2 position = {meshData["position"][0], meshData["position"][1]};
                const float width = meshData["width"];
                const float height = meshData["height"];
                MeshLoader::InitializeRoundedRect(mesh, position.x, position.y, position.x + width,
                                                  position.y - height, meshData["radius"], meshData["segments"]);
            }
        } else if (meshData["type"] == "3D") {
            m_3DMeshes.emplace_back(std::make_unique<Mesh3D>());
            auto &mesh = *m_3DMeshes.back();
            if (meshData["shape"] == "cube") {
                glm::vec3 position = {meshData["position"][0], meshData["position"][1], meshData["position"][2]};
                MeshLoader::InitializeCube(mesh, position, meshData["size"]);
            } else if (meshData["shape"] == "model") {
                MeshLoader::LoadModel(mesh, meshData["modelPath"], true);
                if (meshData.contains("texturePaths")) {
                    if (meshData["texturePaths"].contains("albedo"))
                        mesh.GetTextureLoader().UploadAlbedoTexture(commandPool, meshData["texturePaths"]["albedo"]);
                    if (meshData["texturePaths"].contains("normal"))
                        mesh.GetTextureLoader().UploadNormalTexture(commandPool, meshData["texturePaths"]["normal"]);
                    if (meshData["texturePaths"].contains("gloss"))
                        mesh.GetTextureLoader().UploadGlossTexture(commandPool, meshData["texturePaths"]["gloss"]);
                    if (meshData["texturePaths"].contains("specular"))
                        mesh.GetTextureLoader().UploadSpecularTexture(commandPool,
                                                                      meshData["texturePaths"]["specular"]);
                    mesh.SetPBRMaterial();
                }
                glm::vec3 position = {meshData["position"][0], meshData["position"][1], meshData["position"][2]};
                mesh.GetTransform().Translate(position);
            } else if (meshData["shape"] == "sphere") {
                glm::vec3 position = {meshData["position"][0], meshData["position"][1], meshData["position"][2]};
                MeshLoader::InitializeSphere(mesh, position, meshData["radius"]);
            }
            if (meshData.contains("rotation"))
                mesh.GetTransform().Rotate(
                        {meshData["rotation"][0], meshData["rotation"][1], meshData["rotation"][2]});
            if (meshData.contains("rotationPerSecond"))
                mesh.GetTransform().SetRotationPerSecond(
                        {meshData["rotationPerSecond"][0], meshData["rotationPerSecond"][1], meshData["rotationPerSecond"][2]});
            if (meshData.contains("scale"))
                mesh.GetTransform().Scale({meshData["scale"][0], meshData["scale"][1], meshData["scale"][2]});
        }
    }
}