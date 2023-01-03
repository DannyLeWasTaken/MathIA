//
// Created by Danny Le on 2023-01-02.
//
#include "mesh.hpp"
#include <vector>
#include <iostream>

#include <tiny_obj_loader.h>



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

    if (!msg.empty())
    {
        std::cerr << msg << std::endl;
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
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

                vertices.push_back(new_vert);
                new_triangle.vertices[v] = new_vert;
            }
            triangles.push_back(new_triangle);
            index_offset += fv;
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
