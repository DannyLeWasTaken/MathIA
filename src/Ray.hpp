//
// Created by Danny Le on 2022-12-29.
//

#ifndef MATHIA_RAY_HPP
#define MATHIA_RAY_HPP

#include <glm/vec3.hpp>

class Ray {
public:
    glm::dvec3 origin;
    glm::dvec3 direction;
    double length;

    Ray(const glm::dvec3& origin, const glm::dvec3& direction)
        : origin(origin), direction(direction)
    {}

    glm::dvec3 at(double t) const;
};


#endif //MATHIA_RAY_HPP
