#pragma once
#include "Object.hpp"
#include <vector>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float2.hpp>
#include <GL/glew.h>
#include <functional>
#include "Material.hpp"
class Object;

class Mesh :
    public Object { 
public:
    std::vector<GLfloat> vertices;
    GLuint vertexBuffer = 0;
    std::vector<std::vector<uint32_t>> indices; 
    std::vector<Material> materials;
    GLuint *indiceBuffer;
    GLuint triangleCount = 0;
    std::vector<glm::vec3> colors;
    std::vector<glm::vec3> normals;
    GLuint normalBuffer;
    std::vector<glm::vec2> uvs;
    GLuint uvBuffer;

    bool CanUpdateBuffers = true;

    void SetVertices(const std::vector<GLfloat>& verts);
    void SetIndice(const std::vector<uint32_t>& tris, int idx);
    void SetIndices(const std::vector<std::vector<uint32_t>>& tris);
    void AddIndices(const std::vector<uint32_t>& tris);
    void SetNormals(const std::vector<glm::vec3>& normals);
    void SetUVs(const std::vector<glm::vec2>& uvs);
    void UpdateBuffers();

    void RecalculateNormals(void);
    void SmoothNormals(int repeat, float factor);
    void Optimize(void);

    static Mesh CreatePrimitiveBox(float size);
    static Mesh CreatePrimitiveBox(float sizeX, float sizeY, float sizeZ);
    static Mesh CreatePrimitivePlane(float size);
    static Mesh CreatePrimitivePlane(float sizeX, float sizeY);
    static Mesh CreatePrimitiveIcoSphere(float radius, uint8_t subdivision = 1);

    static Mesh CreateFromAlgorithm(int dimension, int size, int detailLevel, std::function<float (float,float)> func, bool canUpdateBuffer = true);

    static Mesh LoadOBJ(std::string path);
};

void AddVertices(std::vector<GLfloat>& nVert, float* v0, float* v1, float* v2);
