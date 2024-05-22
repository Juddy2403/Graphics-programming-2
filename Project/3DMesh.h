#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "DescriptorPool.h"
#include "DataBuffer.h"
#include "Vertex.h"
#include "Texture.h"


#include "tiny_obj_loader.h"

class DataBuffer;
class Mesh3D
{
private:
    static Texture m_DefaultTexture;

	std::unique_ptr<DataBuffer> m_VertexBuffer{};
    std::unique_ptr<DataBuffer> m_IndexBuffer{};
    std::unordered_map<Vertex3D, uint32_t> m_UniqueVertices{};
    std::vector<Vertex3D> m_Vertices = {};
    std::vector<uint32_t> m_Indices = {};
    glm::mat4 m_WorldMatrix{glm::mat4(1)};
    glm::mat4 m_RotationMatrix{glm::mat4(1.f)};
    glm::mat4 m_TranslationMatrix{glm::mat4(1.f)};
    glm::mat4 m_ScaleMatrix{glm::mat4(1.f)};

    DescriptorPool m_DescriptorPool;
    Texture m_AlbedoTexture{};
    Texture m_NormalTexture{};
    Texture m_GlossTexture{};
    Texture m_SpecularTexture{};
    int m_HasNormalMap = 0;
    void DestroyBuffers();

    void AddVertex(const Vertex3D &vertex);
    void AddRectPlane(Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight, Vertex3D &bottomRight,
                      bool isClockWise, bool keepNormals);
    [[nodiscard]] static Vertex3D GetVertexByIndex(const tinyobj::attrib_t &attrib,const tinyobj::index_t& index) ;

public:
    static void LoadDefaultTexture(VkCommandPool const &commandPool, const std::string &path);
    static void UnloadDefaultTexture();

    Mesh3D();
    explicit Mesh3D(std::vector<Vertex3D>&& vertices, std::vector<uint32_t>&& indices);
    void LoadModel(const std::string &path, bool triangulate);
    Mesh3D(const Mesh3D& other) = delete;
    Mesh3D(Mesh3D&& other) = delete;
    Mesh3D& operator=(const Mesh3D& other) = delete;
    Mesh3D& operator=(Mesh3D&& other) = delete;
    ~Mesh3D() = default;

	void Update(uint32_t currentFrame, UniformBufferObject ubo);
    void ResetVertices(std::vector<Vertex3D>&& vertices);
    void ResetIndices(std::vector<uint32_t>&& indices);
    void Translate(const glm::vec3& translation);
    void Rotate(const glm::vec3& rotation);
    void Scale(const glm::vec3& scale);
	void UploadMesh(const VkCommandPool& commandPool, const VkQueue& graphicsQueue);
    void MapBuffers();

    void InitializeCube(const glm::vec3 &bottomLeftBackCorner, float sideLength);
    void InitializeSphere(const glm::vec3& center, float radius);

    void Destroy();
	void draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const;

    void UploadAlbedoTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadNormalTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadGlossTexture(VkCommandPool const &commandPool, const std::string &path);
    void UploadSpecularTexture(VkCommandPool const &commandPool, const std::string &path);

};
