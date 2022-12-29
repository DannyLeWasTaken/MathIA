//
// Created by Danny Le on 2022-12-29.
//

#ifndef MATHIA_VERTEX_HPP
#define MATHIA_VERTEX_HPP

#include <glm/vec3.hpp>

struct Vertex
{
    glm::dvec3 position;
    glm::dvec3 normal;
};

struct Triangle
{
    Vertex vertices[3];
};


#endif //MATHIA_VERTEX_HPP
