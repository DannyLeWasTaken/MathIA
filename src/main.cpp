#include <iostream>
#include <glm/glm.hpp>
#include "ray.hpp"
#include "vertex.hpp"

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

// Calculate color of a ray
glm::dvec3 ray_color(const ray& r)
{
    // Moller-Trumbore intersection algorithm


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
