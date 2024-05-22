#include "3DMesh.h"
#include <vulkanbase/VulkanUtil.h>
#include <vulkanbase/VulkanBase.h>

#define TINYOBJLOADER_IMPLEMENTATION

#include <tiny_obj_loader.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

//TODO: move this outta here
glm::vec3 Reject(const glm::vec3 &a, const glm::vec3 &b) {
    const glm::vec3 proj = glm::dot(a, b) / glm::dot(b, b) * b;
    return a - proj;
}

Texture Mesh3D::m_DefaultTexture;

Mesh3D::Mesh3D(std::vector<Vertex3D> &&vertices, std::vector<uint32_t> &&indices) : Mesh3D() {
    m_Vertices = std::move(vertices);
    m_Indices = std::move(indices);
}

Mesh3D::Mesh3D() {
    m_DescriptorPool.SetAlbedoImageView(m_DefaultTexture.GetTextureImageView());
    m_DescriptorPool.SetNormalImageView(m_DefaultTexture.GetTextureImageView());
    m_DescriptorPool.SetGlossImageView(m_DefaultTexture.GetTextureImageView());
    m_DescriptorPool.SetSpecularImageView(m_DefaultTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();

    m_VertexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    m_IndexBuffer = std::make_unique<DataBuffer>(VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                                                                                   VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void Mesh3D::Update(uint32_t currentFrame, UniformBufferObject ubo) {
    m_Transform.Update();
    ubo.model = m_Transform.GetWorldMatrix();
    m_DescriptorPool.UpdateUniformBuffer(currentFrame, ubo);
}

void Mesh3D::UploadMesh(const VkCommandPool &commandPool, const VkQueue &graphicsQueue) {
    m_VertexBuffer->Upload(commandPool, graphicsQueue);
    m_IndexBuffer->Upload(commandPool, graphicsQueue);
}

void Mesh3D::UploadAlbedoTexture(const VkCommandPool &commandPool, const std::string &path) {
    m_AlbedoTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetAlbedoImageView(m_AlbedoTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();

}

void Mesh3D::AddRectPlane(Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight, Vertex3D &bottomRight,
                          bool isClockWise, bool keepNormals = true) {

    if (!keepNormals) {
        glm::vec3 normal = glm::normalize(
                glm::cross(topLeft.m_Pos - bottomLeft.m_Pos, bottomRight.m_Pos - bottomLeft.m_Pos));
        if (isClockWise) normal *= -1;
        bottomLeft.m_Normal = normal;
        topLeft.m_Normal = normal;
        bottomRight.m_Normal = normal;
        topRight.m_Normal = normal;
    }
    if (isClockWise) {
        AddVertex(bottomLeft);
        AddVertex(topLeft);
        AddVertex(topRight);
        AddVertex(bottomLeft);
        AddVertex(topRight);
        AddVertex(bottomRight);
    } else {
        AddVertex(bottomLeft);
        AddVertex(topRight);
        AddVertex(topLeft);
        AddVertex(bottomLeft);
        AddVertex(bottomRight);
        AddVertex(topRight);
    }

}

void Mesh3D::InitializeCube(const glm::vec3 &bottomLeftBackCorner, float sideLength) {
    m_Vertices.clear();
    m_Indices.clear();

    //calculate center and translate the cube to the center
    glm::vec3 center = bottomLeftBackCorner + sideLength * 0.5f;
    m_Transform.Translate(center);
    const glm::vec3 bottomLeftBackCornerCentered = -sideLength * 0.5f * glm::vec3(1,1,1);

    const glm::vec3 bottomRightBackCorner = bottomLeftBackCornerCentered + sideLength * Camera::RIGHT;
    const glm::vec3 topLeftBackCorner = bottomLeftBackCornerCentered + sideLength * Camera::UP;
    const glm::vec3 topRightBackCorner = bottomRightBackCorner + sideLength * Camera::UP;
    const glm::vec3 bottomLeftFrontCorner = bottomLeftBackCornerCentered + sideLength * Camera::FORWARD;
    const glm::vec3 bottomRightFrontCorner = bottomRightBackCorner + sideLength * Camera::FORWARD;
    const glm::vec3 topLeftFrontCorner = topLeftBackCorner + sideLength * Camera::FORWARD;
    const glm::vec3 topRightFrontCorner = topRightBackCorner + sideLength * Camera::FORWARD;

    Vertex3D bottomLeftBackCornerVertex{bottomLeftBackCornerCentered};
    Vertex3D topRightFrontCornerVertex{topRightFrontCorner};
    Vertex3D bottomRightBackCornerVertex{bottomRightBackCorner};
    Vertex3D topLeftFrontCornerVertex{topLeftFrontCorner};
    Vertex3D bottomLeftFrontCornerVertex{bottomLeftFrontCorner};
    Vertex3D topRightBackCornerVertex{topRightBackCorner};
    Vertex3D bottomRightFrontCornerVertex{bottomRightFrontCorner};
    Vertex3D topLeftBackCornerVertex{topLeftBackCorner};

    AddRectPlane(bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topRightBackCornerVertex, bottomRightBackCornerVertex, false, false); // back plane
    AddRectPlane(bottomLeftBackCornerVertex, bottomLeftFrontCornerVertex,
                 bottomRightFrontCornerVertex, bottomRightBackCornerVertex, true, false); // bottom plane
    AddRectPlane(bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topLeftFrontCornerVertex, bottomLeftFrontCornerVertex, true, false); // left plane
    AddRectPlane(bottomRightBackCornerVertex, topRightBackCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, false, false); // right plane
    AddRectPlane(bottomLeftFrontCornerVertex, topLeftFrontCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, true, false); // front plane
    AddRectPlane(topLeftBackCornerVertex, topLeftFrontCornerVertex, topRightFrontCornerVertex, topRightBackCornerVertex,
                 false, false); // top plane
}

void Mesh3D::AddVertex(const Vertex3D &vertex) {
    if (m_UniqueVertices.count(vertex) == 0) {
        m_UniqueVertices[vertex] = static_cast<uint32_t>(m_Vertices.size());
        m_Vertices.push_back(vertex);
    }
    m_Indices.push_back(m_UniqueVertices[vertex]);
}

void Mesh3D::MapBuffers() {
    m_VertexBuffer->Map(m_Vertices.size() * sizeof(m_Vertices[0]), m_Vertices.data());
    m_IndexBuffer->Map(m_Indices.size() * sizeof(m_Indices[0]), m_Indices.data());
}

void Mesh3D::draw(const VkCommandBuffer &commandBuffer, uint32_t currentFrame) const {

    m_VertexBuffer->BindAsVertexBuffer(commandBuffer);
    m_IndexBuffer->BindAsIndexBuffer(commandBuffer);
    if(Level::m_AreNormalsEnabled == 1)
        vkCmdPushConstants(commandBuffer, GraphicsPipeline::m_PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT,
                           sizeof(glm::vec3), sizeof(int), &m_HasNormalMap);
    else
        vkCmdPushConstants(commandBuffer, GraphicsPipeline::m_PipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT,
                           sizeof(glm::vec3), sizeof(int), &Level::m_AreNormalsEnabled);

    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GraphicsPipeline::m_PipelineLayout, 0, 1,
                            &m_DescriptorPool.GetDescriptorSets()[currentFrame], 0,
                            nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t >(m_Indices.size()), 1, 0, 0, 0);
}

void Mesh3D::ResetVertices(std::vector<Vertex3D> &&vertices) {
    m_Vertices.clear();
    m_Vertices = std::move(vertices);
}

void Mesh3D::ResetIndices(std::vector<uint32_t> &&indices) {
    m_Indices.clear();
    m_Indices = std::move(indices);
}

void Mesh3D::Destroy() {
    DestroyBuffers();
    m_DescriptorPool.DestroyUniformBuffers();
    m_AlbedoTexture.DestroyTexture();
    m_NormalTexture.DestroyTexture();
    m_GlossTexture.DestroyTexture();
    m_SpecularTexture.DestroyTexture();
}

void Mesh3D::DestroyBuffers() {
    m_VertexBuffer->Destroy();
    m_IndexBuffer->Destroy();
}

void Mesh3D::LoadModel(const std::string &path, bool triangulate = true) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;
    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &err, path.c_str(), 0, triangulate)) {
        throw std::runtime_error(err);
    }

    for (const auto &shape: shapes) {
        for (int i = 0; i < shape.mesh.indices.size() - 2; i += 3) {
            Vertex3D vertex0 = GetVertexByIndex(attrib, shape.mesh.indices[i]);
            Vertex3D vertex1 = GetVertexByIndex(attrib, shape.mesh.indices[i + 1]);
            Vertex3D vertex2 = GetVertexByIndex(attrib, shape.mesh.indices[i + 2]);

            glm::vec3 edge1 = vertex1.m_Pos - vertex0.m_Pos;
            glm::vec3 edge2 = vertex2.m_Pos - vertex0.m_Pos;

            glm::vec2 diffX = glm::vec2(vertex1.m_TexCoord.x - vertex0.m_TexCoord.x,
                                        vertex2.m_TexCoord.x - vertex0.m_TexCoord.x);
            glm::vec2 diffY = glm::vec2(vertex1.m_TexCoord.y - vertex0.m_TexCoord.y,
                                        vertex2.m_TexCoord.y - vertex0.m_TexCoord.y);

            float f = 1.0f / (diffX.x * diffY.y - diffY.x * diffX.y);

            glm::vec3 tangent = (edge1 * diffY.y - edge2 * diffY.x) * f;
            //tangent = glm::normalize(tangent);

            vertex0.m_Tangent += glm::normalize(Reject(tangent, vertex0.m_Normal));
            vertex1.m_Tangent += glm::normalize(Reject(tangent, vertex1.m_Normal));
            vertex2.m_Tangent += glm::normalize(Reject(tangent, vertex2.m_Normal));

            //converting from right-handed to left-handed system
            vertex0.m_Pos.z *= -1;
            vertex0.m_Normal.z *= -1;
            vertex0.m_Tangent.z *= -1;
            vertex1.m_Pos.z *= -1;
            vertex1.m_Normal.z *= -1;
            vertex1.m_Tangent.z *= -1;
            vertex2.m_Pos.z *= -1;
            vertex2.m_Normal.z *= -1;
            vertex2.m_Tangent.z *= -1;

            AddVertex(vertex0);
            AddVertex(vertex1);
            AddVertex(vertex2);
        }
    }
}

Vertex3D Mesh3D::GetVertexByIndex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &index) {
    Vertex3D vertex{};

    vertex.m_Pos = {
            attrib.vertices[3 * index.vertex_index + 0],
            attrib.vertices[3 * index.vertex_index + 1],
            attrib.vertices[3 * index.vertex_index + 2]
    };

    vertex.m_Normal = {
            attrib.normals[3 * index.normal_index + 0],
            attrib.normals[3 * index.normal_index + 1],
            attrib.normals[3 * index.normal_index + 2]
    };

    vertex.m_Color = {1.0f, 1.0f, 1.0f};

    vertex.m_TexCoord = {
            attrib.texcoords[2 * index.texcoord_index + 0],
            1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
    };
    return vertex;
}

void Mesh3D::LoadDefaultTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_DefaultTexture.CreateTextureImage(commandPool, path);
}

void Mesh3D::UnloadDefaultTexture() {
    m_DefaultTexture.DestroyTexture();
}

void Mesh3D::InitializeSphere(const glm::vec3 &center, float radius) {
    m_Vertices.clear();
    m_Indices.clear();
    m_Transform.Translate(center);
    const int sectorCount = 36;
    const int stackCount = 18;
    const auto PI = glm::pi<float>();
    const float sectorStep = 2 * PI / sectorCount;
    const float stackStep = PI / stackCount;
    for (int i = 0; i <= stackCount; ++i) {
        float stackAngle = PI / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        for (int j = 0; j <= sectorCount; ++j) {
            float sectorAngle = j * sectorStep;
            float x = xy * cosf(sectorAngle);
            float y = xy * sinf(sectorAngle);
            Vertex3D vertex{};
            vertex.m_Pos = {x , y , z };
            vertex.m_Normal = glm::normalize(vertex.m_Pos);
            vertex.m_Color = {1.0f, 1.0f, 1.0f};

            m_Vertices.push_back(vertex);
        }
    }
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                m_Indices.push_back(k1);
                m_Indices.push_back(k1 + 1);
                m_Indices.push_back(k2);
            }
            if (i != (stackCount - 1)) {
                m_Indices.push_back(k1 + 1);
                m_Indices.push_back(k2 + 1);
                m_Indices.push_back(k2);
            }
        }
    }

}

void Mesh3D::UploadNormalTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_NormalTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetNormalImageView(m_NormalTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
    m_HasNormalMap = 1;
}

void Mesh3D::UploadGlossTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_GlossTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetGlossImageView(m_GlossTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}

void Mesh3D::UploadSpecularTexture(VkCommandPool const &commandPool, const std::string &path) {
    m_SpecularTexture.CreateTextureImage(commandPool, path);
    m_DescriptorPool.DestroyUniformBuffers();
    m_DescriptorPool.SetSpecularImageView(m_SpecularTexture.GetTextureImageView());
    m_DescriptorPool.Initialize();
}






