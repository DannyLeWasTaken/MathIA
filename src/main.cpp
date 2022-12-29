#include <iostream>
#include <glm/glm.hpp>
#include "ray.hpp"
#include "vertex.hpp"
#include <vector>
#include <map>

// CONFIGURATION

// CAMERA
const int RESOLUTION_X = 256;
const int RESOLUTION_Y = 256;
const int ASPECT_RATIO = RESOLUTION_X / RESOLUTION_Y;

const double FOCAL_LENGTH = 1.0;
const double VIEWPORT_HEIGHT = 2.0;
const double VIEWPORT_WIDTH = ASPECT_RATIO * VIEWPORT_HEIGHT;

const glm::dvec3 CAMERA_ORIGIN = glm::dvec3{0,0,0};
const glm::dvec3 HORIZONTAL = glm::dvec3{VIEWPORT_WIDTH, 0, 0};
const glm::dvec3 VERTICAL = glm::dvec3{0, VIEWPORT_HEIGHT, 0};
const glm::dvec3 LOWER_LEFT_CORNER = CAMERA_ORIGIN - HORIZONTAL/2.0 - VERTICAL/2.0 - glm::dvec3{0, 0, FOCAL_LENGTH};

// SCENE
const glm::dvec3 BACKGROUND_COLOR_TOP = glm::dvec3{0.5, 0.7, 1.0};
const glm::dvec3 BACKGROUND_COLOR_BOTTOM = glm::dvec3{1, 1, 1};

// The current scene composed of triangles
std::vector<Triangle> TriangleScene;

struct Fragment
{
    glm::dvec3 color;
    double depth;
};

std::map<int, std::map<int, Fragment>> sceneBuffer;

// Calculate color of a ray
glm::dvec3 ray_color(const ray& r)
{
    // Moller-Trumbore intersection algorithm
    const double EPISILON = 0.0000001;
    double distance = -1;
    Triangle intersectTriangle;

    for (auto triangle: TriangleScene)
    {
        glm::dvec3 vertex0,vertex1,vertex2;
        vertex0 = triangle.vertices[0].position;
        vertex1 = triangle.vertices[1].position;
        vertex2 = triangle.vertices[2].position;
        glm::dvec3 edge1,edge2,h,s,q;
        double a,f,u,v;
        edge1 = vertex1 - vertex0;
        edge2 = vertex2 - vertex0;
        h = glm::cross(r.direction, edge2);
        a = glm::dot(edge1, h);
        if ( a > -EPISILON && a < EPISILON)
            // Ray is parrallel to the triangle, skip
            continue;
        f = 1.0/a;
        s = r.origin - vertex0;
        u = f * glm::dot(s, h);
        if ( u < 0.0 || u > 1.0)
            continue;
        q = glm::cross(s, edge1);
        v = f * glm::dot(r.direction, q);
        if (v < 0.0 || u + v > 1.0)
            continue;
        // Figure out the intersection point on the line
        float t = f * glm::dot(edge2, q);
        if (t > EPISILON && ( distance == -1 || t < distance ))
        {
            distance = t;
            intersectTriangle = triangle;
        }
    }

    glm::dvec3 unit_direction = glm::normalize(r.direction);
    double t = 0.5 * (unit_direction.y + 1.0);
    return (1.0 - t) * BACKGROUND_COLOR_BOTTOM + t * BACKGROUND_COLOR_TOP;
}

int main() {

    // Rendering

    std::cout << "P3\n"
        << RESOLUTION_X
        << ' '
        << RESOLUTION_Y
        << "\n255\n";

    for (int y = RESOLUTION_Y - 1; y >= 0; --y)
    {
        std::cerr << "\rScanlines remaining: " << y << " " << std::flush;
        for (int x = 0; x < RESOLUTION_X; ++x)
        {
            double u = double(x) / (RESOLUTION_X - 1);
            double v = double(y) / (RESOLUTION_Y - 1);

            ray r(CAMERA_ORIGIN, LOWER_LEFT_CORNER + u*HORIZONTAL + v*VERTICAL - CAMERA_ORIGIN);
            glm::dvec3 output_pixel = ray_color(r);

            // Output color component as RGB [0, 255]
            std::cout << static_cast<int>(255.999 * output_pixel.x) << " "
                    << static_cast<int>(255.999 * output_pixel.y) << " "
                    << static_cast<int>(255.999 * output_pixel.z) << " "
                    << "\n";

        }
    }
    std::cerr << "\nDone.\n";
}
