//
// Created by Danny Le on 2023-01-02.
//
#include "mesh.hpp"
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <chrono>

#include <tiny_obj_loader.h>

glm::dvec3 sampleRandomColor() {
    int index = rand();
    int n = 16;
    return glm::dvec3{
            double(index % n) / double(n - 1),
            double(int(index / n) % n) / double(n - 1),
            double(int(index / (n * n)) % n) / double(n - 1)
    };
}


bool Mesh::load_from_obj(const char* filename) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string msg;

    tinyobj::LoadObj(
            &attrib,
            &shapes,
            &materials,
            &msg,
            filename,
            nullptr,
            true
            );

    if (!msg.empty() || shapes.empty())
    {
        std::cerr << msg << std::endl;
    }


    int i = 0;
    for (size_t s = 0; s < shapes.size(); s++)
    {
        glm::dvec3 shapeColor;
        srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        srand(double(s) * double(s) * double(rand()) * 3.145 );
        while (true) {
            shapeColor = sampleRandomColor();
            shapeColor = glm::normalize(shapeColor);
            if (glm::length(shapeColor)*glm::length(shapeColor) > 0.2)
            {
                if ((
                            abs(shapeColor.x - shapeColor.y) +
                            abs(shapeColor.x - shapeColor.z) +
                            abs(shapeColor.x - shapeColor.z) +
                            abs(shapeColor.y - shapeColor.z)) / 4 > 0.2 )
                {
                    break;
                }
            }
        }


        size_t index_offset = 0;
        // iterate over all faces
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            //size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);
            size_t fv = 3;
            Triangle new_triangle;
            // Iterate all vertices in face
            for (size_t v = 0; v < fv; v++) {
                //access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                //Vertex position
                tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
                tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
                tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

                tinyobj::real_t nx,ny,nz;

                //Vertex normal
                if (idx.normal_index >= 0) {
                    nx = attrib.normals[3 * idx.normal_index + 0];
				    ny = attrib.normals[3 * idx.normal_index + 1];
				    nz = attrib.normals[3 * idx.normal_index + 2];
                } else {
                    std::cerr << "Mesh: " << filename << " does not have normals for its vertices!" << std::endl;
                }

                //Copy it into the new vertex struct
                Vertex new_vert;
                new_vert.position.x = vx;
                new_vert.position.y = vy;
                new_vert.position.z = vz;

                new_vert.normal.x = nx;
                new_vert.normal.y = ny;
                new_vert.normal.z = nz;

                new_vert.shapeId  = s;
                new_vert.color = shapeColor;

                vertices.push_back(new_vert);
                new_triangle.vertices[v] = new_vert;
            }
            new_triangle.index = i;
            new_triangle.shapeId = s;
            new_triangle.triangleColor = sampleRandomColor();
            triangles.push_back(new_triangle);
            i++;
            index_offset += fv;
            shapes[s].mesh.material_ids[f];
        }
    }
    return true;
}

const std::vector<Vertex> &Mesh::getVertices() const {
    return vertices;
}

const std::vector<Triangle> &Mesh::getTriangles() const {
    return triangles;
}

const glm::dmat4 &Mesh::getTransformMatrix() const {
    return transformMatrix;
}

const glm::dvec3 &Mesh::getPosition() const {
    return position;
}

void Mesh::setPosition(const glm::dvec3 &position) {
    Mesh::position = position;
    glm::dmat4 locationMatrix = glm::translate(position);
    glm::mat4 angleMatrix = glm::rotate(
            locationMatrix,
            (double)glm::radians(360.f),
            rotation
            );
    transformMatrix = angleMatrix;
}
