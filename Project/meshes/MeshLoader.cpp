#include "MeshLoader.h"
#include "meshes/3DMesh.h"
#include "meshes/2DMesh.h"
#include "Camera.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "lib/tiny_obj_loader.h"

glm::vec3 MeshLoader::Reject(const glm::vec3 &a, const glm::vec3 &b) {
    const glm::vec3 proj = glm::dot(a, b) / glm::dot(b, b) * b;
    return a - proj;
}

void MeshLoader::LoadModel(Mesh3D &mesh, const std::string &path, bool triangulate) {
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

            mesh.AddVertex(vertex0);
            mesh.AddVertex(vertex1);
            mesh.AddVertex(vertex2);
        }
    }
}

Vertex3D MeshLoader::GetVertexByIndex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &index) {
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

void MeshLoader::InitializeCube(Mesh3D &mesh, const glm::vec3 &bottomLeftBackCorner, float sideLength) {
    mesh.ClearIndices();
    mesh.ClearVertices();

    //calculate center and translate the cube to the center
    glm::vec3 center = bottomLeftBackCorner + sideLength * 0.5f;
    mesh.GetTransform().Translate(center);

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

    AddRectPlane(mesh,bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topRightBackCornerVertex, bottomRightBackCornerVertex, false); // back plane
    AddRectPlane(mesh,bottomLeftBackCornerVertex, bottomLeftFrontCornerVertex,
                 bottomRightFrontCornerVertex, bottomRightBackCornerVertex, true); // bottom plane
    AddRectPlane(mesh,bottomLeftBackCornerVertex, topLeftBackCornerVertex,
                 topLeftFrontCornerVertex, bottomLeftFrontCornerVertex, true); // left plane
    AddRectPlane(mesh,bottomRightBackCornerVertex, topRightBackCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, false); // right plane
    AddRectPlane(mesh,bottomLeftFrontCornerVertex, topLeftFrontCornerVertex,
                 topRightFrontCornerVertex, bottomRightFrontCornerVertex, true); // front plane
    AddRectPlane(mesh,topLeftBackCornerVertex, topLeftFrontCornerVertex, topRightFrontCornerVertex, topRightBackCornerVertex,
                 false); // top plane
}

void MeshLoader::InitializeSphere(Mesh3D &mesh, const glm::vec3 &center, float radius) {
    mesh.ClearIndices();
    mesh.ClearVertices();
    mesh.GetTransform().Translate(center);
    constexpr int sectorCount = 36;
    constexpr int stackCount = 18;
    const auto PI = glm::pi<float>();
    const float sectorStep = 2 * PI / sectorCount;
    const float stackStep = PI / stackCount;
    std::vector<Vertex3D> tempVertices;
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
            tempVertices.push_back(vertex);
        }
    }
    for (int i = 0; i < stackCount; ++i) {
        int k1 = i * (sectorCount + 1);
        int k2 = k1 + sectorCount + 1;
        for (int j = 0; j < sectorCount; ++j, ++k1, ++k2) {
            if (i != 0) {
                mesh.AddVertex(tempVertices[k1]);
                mesh.AddVertex(tempVertices[k1 + 1]);
                mesh.AddVertex(tempVertices[k2]);
            }
            if (i != (stackCount - 1)) {
                mesh.AddVertex(tempVertices[k1 + 1]);
                mesh.AddVertex(tempVertices[k2 + 1]);
                mesh.AddVertex(tempVertices[k2]);
            }
        }
    }
}

void MeshLoader::AddRectPlane(Mesh3D &mesh, Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight,
                              Vertex3D &bottomRight, bool isClockWise, bool keepNormals) {
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
        mesh.AddVertex(bottomLeft);
        mesh.AddVertex(topLeft);
        mesh.AddVertex(topRight);
        mesh.AddVertex(bottomLeft);
        mesh.AddVertex(topRight);
        mesh.AddVertex(bottomRight);
    } else {
        mesh.AddVertex(bottomLeft);
        mesh.AddVertex(topRight);
        mesh.AddVertex(topLeft);
        mesh.AddVertex(bottomLeft);
        mesh.AddVertex(bottomRight);
        mesh.AddVertex(topRight);
    }
}

void MeshLoader::InitializeRect(Mesh2D &mesh, const glm::vec2 &bottomLeft, float sideLen) {
    mesh.ClearIndices();
    mesh.ClearVertices();
    AddRect(mesh,bottomLeft.y + sideLen, bottomLeft.x, bottomLeft.y, bottomLeft.x + sideLen);
}

void MeshLoader::InitializeCircle(Mesh2D &mesh, const glm::vec2 &center, float radius, int nrOfSegments) {
    mesh.ClearIndices();
    mesh.ClearVertices();
    const float angleIncrement = 2.0f * glm::pi<float>() / nrOfSegments;

    glm::vec2 pos1 = {radius + center.x, center.y};

    for (int i = 1; i <= nrOfSegments; ++i) {
        const float angle = angleIncrement * i;
        glm::vec2 pos2 = {radius * cos(angle) + center.x, radius * sin(angle) + center.y};

        // Add vertex positions to the vertex buffer
        mesh.AddVertex(pos2, {1.0f, 1.0f, 1.0f});
        mesh.AddVertex(pos1,{1.0f, 1.0f, 1.0f});
        mesh.AddVertex(center,{1.0f, 0.0f, 0.0f});

        pos1 = pos2;
    }
}

void MeshLoader::InitializeRoundedRect(Mesh2D &mesh, float left, float bottom, float right, float top, float radius,
                                       int nrOfSegments) {
    mesh.ClearIndices();
    mesh.ClearVertices();
    // Calculate the corner vertices
    const std::vector<glm::vec2> corners = {
            {right, bottom},     // Top-right     0
            {left,  bottom},      // Top-left      1
            {left,  top},  // Bottom-left   2
            {right, top}     // Bottom-right  3
    };

    std::vector<glm::vec2> pos1;
    std::vector<glm::vec2> pos2;

    constexpr float rightAngle = glm::pi<float>() / 2.f;
    const float angleIncrement = rightAngle / nrOfSegments;

    for (size_t i = 0; i < 4; i++) {
        pos1.emplace_back(corners[i].x + radius * cos((angleIncrement * 0 + rightAngle * i)),
                          corners[i].y + radius * sin((angleIncrement * 0 + rightAngle * i)));
    }
    AddRect(mesh,pos1[1].y, pos1[1].x, pos1[3].y, pos1[3].x);
    AddRect(mesh,pos1[0].y, pos1[2].x, pos1[2].y, pos1[0].x);

    for (int j = 1; j <= nrOfSegments; ++j) {
        pos2.clear();

        for (size_t i = 0; i < 4; i++) {
            pos2.emplace_back(corners[i].x + radius * cos((angleIncrement * j + rightAngle * i)),
                              corners[i].y + radius * sin((angleIncrement * j + rightAngle * i)));

            mesh.AddVertex(pos1[i],{1.0f, 0.0f, 0.0f});
            mesh.AddVertex(corners[i],{1.0f, 0.0f, 0.0f});
            mesh.AddVertex(pos2[i],{1.0f, 0.0f, 0.0f});

        }
        pos1.clear();
        pos1.insert(pos1.begin(), pos2.begin(), pos2.end());
    }
}

void MeshLoader::AddRect(Mesh2D &mesh, float top, float left, float bottom, float right) {
    mesh.AddVertex(glm::vec2(left, bottom),{1.0f, 0.0f, 0.0f});
    mesh.AddVertex(glm::vec2(left, top),{0.0f, 0.0f, 1.0f});
    mesh.AddVertex(glm::vec2(right, top),{0.0f, 1.0f, 0.0f});

    mesh.AddVertex(glm::vec2(left, bottom),{1.0f, 0.0f, 0.0f});
    mesh.AddVertex(glm::vec2(right, top),{1.0f, 1.0f, 1.0f});
    mesh.AddVertex(glm::vec2(right, bottom),{0.0f, 1.0f, 0.0f});
}
