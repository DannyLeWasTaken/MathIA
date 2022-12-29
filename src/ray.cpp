//
// Created by Danny Le on 2022-12-29.
//

#include "ray.hpp"

glm::dvec3 ray::at(double t) const {
    return origin + t*direction;
}