#pragma once
#include <string>
#include <glm/detail/type_vec3.hpp>
#include <glm/vec3.hpp>
#include "Vertex.h"
#include "tiny_obj_loader.h"

class Mesh3D;
class Mesh2D;
class MeshLoader {
private:
    static glm::vec3 Reject(const glm::vec3 &a, const glm::vec3 &b);
    [[nodiscard]] static Vertex3D GetVertexByIndex(const tinyobj::attrib_t &attrib, const tinyobj::index_t &index);
    static void AddRectPlane(Mesh3D &mesh, Vertex3D &bottomLeft, Vertex3D &topLeft, Vertex3D &topRight,
                             Vertex3D &bottomRight, bool isClockWise, bool keepNormals = false);

    static void AddRect(Mesh2D &mesh,float top, float left, float bottom, float right);
public:
    //3D Mesh
    static void LoadModel(Mesh3D &mesh, const std::string &path, bool triangulate);
    static void InitializeCube(Mesh3D &mesh, const glm::vec3 &bottomLeftBackCorner, float sideLength);
    static void InitializeSphere(Mesh3D &mesh, const glm::vec3 &center, float radius);

    //2D Mesh
    static void InitializeCircle(Mesh2D &mesh,const glm::vec2 &center, float radius, int nrOfSegments);
    static void InitializeRect(Mesh2D &mesh,const glm::vec2 &bottomLeft, float sideLen);
    static void InitializeRoundedRect(Mesh2D &mesh,float left, float bottom, float right, float top, float radius, int nrOfSegments);

    static void LoadModelNoTan(Mesh3D &mesh, const std::string &path, bool triangulate);
};


