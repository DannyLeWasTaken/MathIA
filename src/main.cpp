#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/transform.hpp>
#include "Ray.hpp"
#include "mesh.hpp"
#include <vector>
#include <map>
#include <omp.h>
#include <map>

// CONFIGURATION

// RENDERING POWER
const int MAX_THREAD_RENDER = 8; // Maximum # of threads to be used while rendering

// CAMERA
const int RESOLUTION_X = 256;
const int RESOLUTION_Y = 256;
const double ASPECT_RATIO = double(RESOLUTION_X) / double(RESOLUTION_Y);

const double FOCAL_LENGTH = 1.0;
const double VIEWPORT_HEIGHT = 2.0;
const double VIEWPORT_WIDTH = ASPECT_RATIO * VIEWPORT_HEIGHT;

const glm::dvec3 CAMERA_ORIGIN = glm::dvec3{0,0,0};
const glm::dvec3 HORIZONTAL = glm::dvec3{VIEWPORT_WIDTH, 0, 0};
const glm::dvec3 VERTICAL = glm::dvec3{0, VIEWPORT_HEIGHT, 0};
const glm::dvec3 LOWER_LEFT_CORNER = CAMERA_ORIGIN - HORIZONTAL/2.0 - VERTICAL/2.0 - glm::dvec3{0, 0, FOCAL_LENGTH};

// SCENE
const glm::dvec3 BACKGROUND_COLOR_TOP = glm::dvec3{0.0, 0.0, 1.0};
const glm::dvec3 BACKGROUND_COLOR_BOTTOM = glm::dvec3{1, 1, 1};

glm::dvec3 COLOR_SEQUENCE = glm::dvec3{1.0};

// The current scene composed of Meshes
std::vector<Mesh> Scene;

struct Fragment
{
    glm::dvec3 color;
    double depth;
};

std::map<int, std::map<int, Fragment>> sceneBuffer;

// Calculate color of a Ray
glm::dvec3 ray_color(const Ray& r)
{
    // Moller-Trumbore intersection algorithm
    const double EPISILON = 0.0000001;
    double lastDistance = pow(2, 64) - 1;
    double distance = pow(2, 64) - 1;
    glm::dvec3 pixelColor;
    std::map<double, int> threadDistances;
    std::map<double, Triangle> threadTriangles;
    Triangle intersectingTriangle;

    {
        //#pragma omp parallel default(none) shared(Scene,EPISILON,r,distance,distanceMap)
        for (auto mesh: Scene) {
            threadTriangles.clear();
            threadDistances.clear();

            omp_set_num_threads(MAX_THREAD_RENDER);
            #pragma omp parallel for default(none) reduction(min:distance) shared(Scene,EPISILON,r,mesh,threadTriangles,threadDistances)
            for (auto triangle: mesh.getTriangles()) {
                const int THREAD_ID = omp_get_thread_num();
                glm::dvec3 vertex0, vertex1, vertex2;
                /**
                vertex0 = cameraViewProjection * mesh.getTransformMatrix() * glm::dvec4(triangle.vertices[0].position, 1.0);
                vertex1 = cameraViewProjection * mesh.getTransformMatrix() * glm::dvec4(triangle.vertices[1].position, 1.0);
                vertex2 = cameraViewProjection * mesh.getTransformMatrix() * glm::dvec4(triangle.vertices[2].position, 1.0);
                **/
                //std::cerr << omp_get_thread_num() << std::endl;
                vertex0 = triangle.vertices[0].position + mesh.getPosition();
                vertex1 = triangle.vertices[1].position + mesh.getPosition();
                vertex2 = triangle.vertices[2].position + mesh.getPosition();
                glm::dvec3 edge1, edge2, h, s, q;
                double a, f, u, v;
                edge1 = vertex1 - vertex0;
                edge2 = vertex2 - vertex0;
                h = glm::cross(r.direction, edge2);
                a = glm::dot(edge1, h);
                if (a > -EPISILON && a < EPISILON)
                    // Ray is parrallel to the triangle, skip
                    continue;
                f = 1.0 / a;
                s = r.origin - vertex0;
                u = f * glm::dot(s, h);
                if (u < 0.0 || u > 1.0)
                    continue;
                q = glm::cross(s, edge1);
                v = f * glm::dot(r.direction, q);
                if (v < 0.0 || u + v > 1.0)
                    continue;
                // Figure out the intersection point on the line
                double t = f * glm::dot(edge2, q);
                if (t > EPISILON && t < distance) {
                    distance = t;
                    #pragma omp critical
                    {
                        threadDistances[t] = THREAD_ID;
                        threadTriangles[THREAD_ID] = triangle;
                    };
                }
            }

            if (distance < lastDistance)
            {
                lastDistance = distance;
                intersectingTriangle = threadTriangles[threadDistances[distance]];
                //memcpy(intersectingTriangle, triangles[distances[distance]], sizeof(Triangle));
            }
        }
    };
    if (distance != pow(2, 64) - 1)
    {
        //Triangle triangle = distanceMap[distance];
        /**
        pixelColor = glm::normalize( (
                glm::clamp(intersectingTriangle.vertices[0].normal, 0.0, 1.0) +
                glm::clamp(intersectingTriangle.vertices[1].normal, 0.0, 1.0) +
                glm::clamp(intersectingTriangle.vertices[2].normal, 0.0, 1.0)) / glm::dvec3{3.f});
        **/
        srand(pow((long)intersectingTriangle.index,2) );
        /**
        pixelColor = glm::dvec3{
                pow(double(rand() % 1000) / 1000, 2),
                pow(double(rand() % 1000) / 1000, 2),
                pow(double(rand() % 1000) / 1000, 2)
        };
        **/
        bool foundColor = false;
        while (foundColor == false) {
            int index = rand();
            int n = 12;
            pixelColor = glm::dvec3{
                    double(index % n) / double(n - 1),
                    double(int(index / n) % n) / double(n - 1),
                    double(int(index / (n * n)) % n) / double(n - 1)
            };
            // Gamma correction
            pixelColor = glm::pow(pixelColor, glm::dvec3{1 / 2.2});
            pixelColor = glm::normalize(pixelColor);
            if (glm::length(pixelColor)*glm::length(pixelColor) > 0.2)
            {
                if ((
                        abs(pixelColor.x - pixelColor.y) +
                        abs(pixelColor.x - pixelColor.z) +
                        abs(pixelColor.x - pixelColor.z) +
                        abs(pixelColor.y - pixelColor.z)) / 4 > 0.2 )
                {
                    foundColor = false;
                    break;
                }
            }
        }
        //pixelColor = glm::dvec3{distance};
        //pixelColor = glm::abs(pixelColor);
        //return glm::normalize(glm::dvec3{0,0,0});
    } else {
        glm::dvec3 unit_direction = glm::normalize(r.direction);
        double t = 0.5 * (unit_direction.y + 1.0);
        pixelColor = (1.0 - t) * BACKGROUND_COLOR_BOTTOM + t * BACKGROUND_COLOR_TOP;
    }
    return glm::normalize(pixelColor );
}

void load_scene() {
    Mesh monkey;
    monkey.load_from_obj("C:/Users/Danny Le/CLionProjects/MathIA/assets/monkey_smooth.obj");
    //monkey.position = glm::dvec3{0, 0, -2};
    monkey.setPosition(glm::dvec3{0, 0, -2});
    //Scene.push_back(monkey);

    int numMonkeyGrid = 0;
    for (int x = -numMonkeyGrid; x <= numMonkeyGrid; x++)
    {
        for (int y = -numMonkeyGrid; y <= numMonkeyGrid; y++) {
            Mesh newMonkey;
            newMonkey.load_from_obj("C:/Users/Danny Le/CLionProjects/MathIA/assets/monkey_smooth.obj");
            newMonkey.setPosition(glm::dvec3{x*2, y*2, -2});
            Scene.push_back(newMonkey);
        }
    }

    Mesh cow;
    cow.load_from_obj("C:/Users/Danny Le/CLionProjects/MathIA/assets/Cow.obj");
    //cow.position = glm::dvec3{-1.95, -.5, 0.5};
    //Scene.push_back(cow);

    Mesh cornell;
    cornell.load_from_obj("C:/Users/Danny Le/CLionProjects/MathIA/assets/cornell_box.obj");
    //cornell.position = glm::dvec3{-250,-100,-400};
    cornell.setPosition(glm::dvec3{25, -275, -300});
    Scene.push_back(cornell);
}

int main() {

    // Rendering

    std::cout << "P3\n"
        << RESOLUTION_X
        << ' '
        << RESOLUTION_Y
        << "\n255\n";

    // Scene handling
    load_scene();

    for (int y = RESOLUTION_Y - 1; y >= 0; --y)
    {
        std::cerr << "\rScanlines remaining: " << y << "\n" << std::flush;
        for (int x = 0; x < RESOLUTION_X; ++x)
        {
            double u = double(x) / (RESOLUTION_X - 1);
            double v = double(y) / (RESOLUTION_Y - 1);

            Ray r(CAMERA_ORIGIN, LOWER_LEFT_CORNER + u * HORIZONTAL + v * VERTICAL - CAMERA_ORIGIN);
            glm::dvec3 output_pixel = ray_color(r);

            // Output color component as RGB [0, 255]
            glm::dvec3 postProcessedPixel = output_pixel;
            // Gamma correction
            //postProcessedPixel = glm::pow(output_pixel, glm::dvec3{1/2.2});
            postProcessedPixel = glm::normalize(postProcessedPixel);

            std::cout << static_cast<int>(255.999 * postProcessedPixel.x) << " "
                    << static_cast<int>(255.999 * postProcessedPixel.y) << " "
                    << static_cast<int>(255.999 * postProcessedPixel.z) << " "
                    << "\n";

        }
    }
    std::cerr << "\nDone.\n";
}
