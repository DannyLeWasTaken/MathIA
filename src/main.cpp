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
#include <chrono>

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
    const double EPSILON = 0.0000001;
    //const double EPSILON = 1.0;
    double lastDistance = pow(2, 64) - 1;
    double distance = pow(2, 64) - 1;
    glm::dvec3 pixelColor;

    struct triangleHitInfo {
        double distance;
        double u;
        double v;
        Triangle triangle;
    };

    std::map<double, int> threadDistances;
    std::map<double, triangleHitInfo> threadTriangles;
    triangleHitInfo intersectingTriangle{};

    {
        //#pragma omp parallel default(none) shared(Scene,EPSILON,r,distance,distanceMap)
        for (const auto& mesh: Scene) {
            threadTriangles.clear();
            threadDistances.clear();

            // Multi-thread!
            omp_set_num_threads(MAX_THREAD_RENDER);
            #pragma omp parallel for default(none) reduction(min:distance) shared(Scene,EPSILON,r,mesh,threadTriangles,threadDistances)
            for (auto triangle: mesh.getTriangles()) {
                const int THREAD_ID = omp_get_thread_num();
                glm::dvec3 vertex0, vertex1, vertex2;
                glm::dvec3 edge1, edge2, tvec, pvec, qvec;
                double det, inv_det;
                // Offset each vertex by the mesh's position
                vertex0 = triangle.vertices[0].position + mesh.getPosition();
                vertex1 = triangle.vertices[1].position + mesh.getPosition();
                vertex2 = triangle.vertices[2].position + mesh.getPosition();

                edge1 = vertex1 - vertex0;
                edge2 = vertex2 - vertex0;

                // Calculate the determinant
                pvec = glm::cross(r.direction, edge2);

                // If determinant is close to zero, it is in the plane of the triangle
                det = glm::dot(edge1, pvec);

                if (det > -EPSILON && det < EPSILON)
                    continue;
                inv_det = 1.0 / det;

                tvec = r.origin - vertex0;

                // Check if u or v is out of range [0, 1]
                double u = glm::dot(tvec, pvec) * inv_det;
                if (u < EPSILON || u > 1.0)
                    continue;

                qvec = glm::cross(tvec, edge1);

                double v = glm::dot(r.direction, qvec) * inv_det;
                if (v < EPSILON || u + v > 1.0)
                    continue;

                double t = glm::dot(edge2, qvec) * inv_det;
                if (t > EPSILON && t < distance) {
                    distance = t;
                    #pragma omp critical
                    {
                        threadDistances[t] = THREAD_ID;
                        triangleHitInfo triInfoStruct{};
                        triInfoStruct.distance = t;
                        triInfoStruct.triangle = triangle;
                        triInfoStruct.u = u;
                        triInfoStruct.v = v;
                        threadTriangles[THREAD_ID] = triInfoStruct;
                    }
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
        // Shading the pixel/
        auto u = intersectingTriangle.u;
        auto v = intersectingTriangle.v;
        auto w = 1 - u - v;
        pixelColor = glm::dvec3{1} - ((intersectingTriangle.triangle.vertices[1].normal * u) +
                (intersectingTriangle.triangle.vertices[2].normal * v) +
                (intersectingTriangle.triangle.vertices[0].normal * w ));
        pixelColor *= intersectingTriangle.triangle.color;
        //pixelColor = glm::dvec3{intersectingTriangle.u, intersectingTriangle.v, 1 - intersectingTriangle.u - intersectingTriangle.v};
        //pixelColor = intersectingTriangle.triangle.triangleColor;
        pixelColor /= std::abs(intersectingTriangle.distance*intersectingTriangle.distance*intersectingTriangle.distance);
        pixelColor = glm::pow(pixelColor, glm::dvec3{1/2.2});
    } else {
        glm::dvec3 unit_direction = glm::normalize(r.direction);
        double t = 0.5 * (unit_direction.y + 1.0);
        pixelColor = (1.0 - t) * BACKGROUND_COLOR_BOTTOM + t * BACKGROUND_COLOR_TOP;
        pixelColor = glm::pow(pixelColor, glm::dvec3{1/2.2});
    }
    return pixelColor;
}

void load_scene() {
    Mesh monkey;
    //monkey.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/monkey_smooth.obj");
    //monkey.position = glm::dvec3{0, 0, -2};
    monkey.setPosition(glm::dvec3{0, 0, -2});
    //Scene.push_back(monkey);

    /**
    int numMonkeyGrid = 1;
    int totalMonkeys = 0;
    for (int x = -numMonkeyGrid; x <= numMonkeyGrid; x++)
    {
        for (int y = -numMonkeyGrid; y <= numMonkeyGrid; y++) {
            Mesh newMonkey;
            newMonkey.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/monkey_smooth.obj");
            newMonkey.setPosition(glm::dvec3{x*2, y*2, -6});
            Scene.push_back(newMonkey);
            totalMonkeys++;
        }
    }
    **/


    //Mesh cow;
    //cow.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/Cow.obj");
    //cow.setPosition(glm::dvec3{0, 0, -2});
    //Scene.push_back(cow);

    Mesh cornell;
    cornell.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/cornell.obj");
    cornell.setPosition(glm::dvec3{25,-275,-300});
    Scene.push_back(cornell);

    Mesh sponza;
    //sponza.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/sponza.obj");
    sponza.setPosition(glm::dvec3{0, -1, 0});
    //Scene.push_back(sponza);

    Mesh helmet;
    //helmet.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/helmet.obj");
    helmet.setPosition(glm::dvec3{0, 0, -2});
    //Scene.push_back(helmet);

    Mesh chess;
    //chess.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/chess.obj");
    chess.setPosition(glm::dvec3{0, -.2, -.6});
    //Scene.push_back(chess);

    Mesh minecraft;
    //minecraft.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/forest.obj");
    //minecraft.setPosition(glm::dvec3{0, -3.656, -62.0128});
    //minecraft.setPosition(glm::dvec3{0 , -4.0165, -20.3279});
    //Scene.push_back(minecraft);

    Mesh cyberpunk;
    cyberpunk.load_from_obj("C:/Users/Danny/CLionProjects/MathIA/assets/cyberpunkcar.obj");
    //minecraft.setPosition(glm::dvec3{0, -3.656, -62.0128});
    cyberpunk.setPosition(glm::dvec3{0 , -1.5, -5});
    Scene.push_back(cyberpunk);

    int num_triangles = 0;
    for (auto& mesh: Scene) {
        num_triangles += mesh.getTriangles().size();
    }
    std::cerr << "Num triangles: " + std::to_string(num_triangles) << std::endl;
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

    auto start_time = std::chrono::high_resolution_clock::now();
    for (int y = RESOLUTION_Y - 1; y >= 0; --y)
    {
        std::cerr << "\rScanlines remaining: " << y << "\n" << std::flush;
        for (int x = 0; x < RESOLUTION_X; ++x)
        {
            double u = double(x) / (RESOLUTION_X - 1);
            double v = double(y) / (RESOLUTION_Y - 1);

            glm::dvec3 r_d = glm::dvec3{
                u * VIEWPORT_WIDTH,
                v * VIEWPORT_HEIGHT,
                0
            };
            r_d -= glm::dvec3{
                VIEWPORT_WIDTH / 2,
                VIEWPORT_HEIGHT / 2,
                FOCAL_LENGTH
            };

            Ray r(CAMERA_ORIGIN, r_d);
            //Ray r(CAMERA_ORIGIN, -HORIZONTAL/2.0 - VERTICAL/2.0 - glm::dvec3{0, 0, FOCAL_LENGTH} + u * HORIZONTAL + v * VERTICAL);
            //Ray r(CAMERA_ORIGIN, LOWER_LEFT_CORNER + u * HORIZONTAL + v * VERTICAL - CAMERA_ORIGIN);
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
    auto end_time = std::chrono::high_resolution_clock::now();
    std::cerr << "\nDone. (" +
    std::to_string(   double(std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count())/double(1000)) + "s)\n";
}
