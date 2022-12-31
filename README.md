# Math IA: Rendering triangle meshes through the Möller-Trumbore Intersection Algorithm 
This is for my math IA for my IB math course (2022-2023).
The goal of this project is to be able to render mesh
triangles through ray-triangle intersections.

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
