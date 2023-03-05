//
// Created by Danny Le on 2023-01-02.
//

#ifndef MATHIA_MESH_HPP
#define MATHIA_MESH_HPP
#include <glm/glm.hpp>
#include <string>
#include <vector>


struct Vertex
{
    glm::dvec3 position;
    glm::dvec3 normal;
    unsigned int shapeId;
    glm::dvec3 color;
};

struct Triangle
{
    Vertex vertices[3];
    int index;
    unsigned int shapeId;
    glm::dvec3 color;
    glm::dvec3 triangleColor;
};

class Mesh {
public:
    bool load_from_obj(const char* filename);
private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    glm::dvec3 position;
    glm::dvec3 rotation;
public:
    void setPosition(const glm::dvec3 &position);

public:
    const glm::dvec3 &getPosition() const;

private:
    glm::dmat4 transformMatrix;
public:
    const glm::dmat4 &getTransformMatrix() const;

public:
    const std::vector<Triangle> &getTriangles() const;

public:
    const std::vector<Vertex> &getVertices() const;
};


#endif //MATHIA_MESH_HPP
