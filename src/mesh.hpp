//
// Created by Danny Le on 2023-01-02.
//

#ifndef MATHIA_MESH_HPP
#define MATHIA_MESH_HPP
#include <glm/vec3.hpp>
#include <string>
#include <vector>

struct Vertex
{
    glm::dvec3 position;
    glm::dvec3 normal;
};

struct Triangle
{
    Vertex vertices[3];
};

class Mesh {
public:
    bool load_from_obj(std::string filename);
private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
public:
    const std::vector<Triangle> &getTriangles() const;

public:
    const std::vector<Vertex> &getVertices() const;
};


#endif //MATHIA_MESH_HPP
