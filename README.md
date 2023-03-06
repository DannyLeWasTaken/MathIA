# Math IA: Rendering triangle meshes through the Möller-Trumbore Intersection Algorithm 
This is for my math IA for my IB math course (2022-2023).
The goal of this project is to be able to render mesh
triangles through Ray-triangle intersections.

## Limitations
There are several limitations posed by this algorithm:
1. No form of acceleration structure. This would mean 
our algorithm would blindly check every triangle in the scene. This can be improved by using
octrees.
2. No triangle culling. This means every triangle even if it's
occluded by another triangle, will have the intersection algorithm performed
upon it.
3. One-sided intersection of Moller-Trumbore intersection algorithm.
Implementing this allows you to create certain optimization that would make the algorithm faster.

# Build instructions
In the directory of the project:
- cmake ./ build
- Import asset of your choice
- Go into build directory
- Run: ``[BUILD PATH]\MathIA.exe >> image.ppm``
- Open the ``image.ppm``` in desired ppm viewer

## Ideal build instructions
- Pull the project from VSC using CLion
- Load the CMake file
- ``.\cmake-build-debug\MathIA.exe >> image.ppm``
- Open ``image.ppm`` in desired ppm viewer

## To modify
- To modify this project, look in ``main.cpp`` and mainly ``load_scene()``

## How shading works

`t = Ray distance`
- `((v1.Normal * u + v2.Normal * v + v3.Normal * w) * triangle.Color / abs(t))^(1/2.2)`
- Tends to produce "good enough" differences in meshes so that it's usable to show off diagrams
- There is however an optional method to comment out all said shading and set ``pixelColor`` to: ``intersectingTriangle.triangle.triangleColor``
  - Triangle color represents a unique color assigned to the triangle upon load
  - Color represents a unique color assigned to the triangle's shape upon load