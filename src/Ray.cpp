//
// Created by Danny Le on 2022-12-29.
//

#include "Ray.hpp"

glm::dvec3 Ray::at(double t) const {
    return origin + t*direction;
}