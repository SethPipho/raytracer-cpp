#ifndef GEOMETERY_H_
#define GEOMETERY_H_

#include <iostream>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "ray.h"

class Mesh {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<int> face_indices;
       
        Mesh(){};
};


class Triangle{
    public:
        Mesh* mesh;
        int face_offest;
        Triangle(Mesh* mesh, int face_offset): mesh(mesh), face_offest(face_offest){};
};


//Möller–Trumbore intersection algorithm
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
inline bool rayTriangleIntersection(Ray &ray,Triangle &triangle){

    int face_offset = triangle.face_offest;
    Mesh* mesh = triangle.mesh;
    
    glm::vec3 v0 = mesh->vertices[mesh->face_indices[face_offset]];
    glm::vec3 v1 = mesh->vertices[mesh->face_indices[face_offset + 1]];
    glm::vec3 v2 = mesh->vertices[mesh->face_indices[face_offset + 2]];

    glm::vec3 v0v1 = v1 - v0; 
    glm::vec3 v0v2 = v2 - v0; 
    glm::vec3 normal = glm::normalize(glm::cross(v0v1, v0v2));
    glm::vec3 pvec = glm::cross(ray.direction, v0v2);
    float det = glm::dot(v0v1, pvec); 

    

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 0.000001) return false; 

    float invDet = 1 / det; 
 
    glm::vec3 tvec = ray.origin - v0; 
    float u = glm::dot(tvec, pvec) * invDet; 
    if (u < 0 || u > 1) return false; 
 
    glm::vec3 qvec = glm::cross(tvec, v0v1); 
    float v = glm::dot(ray.direction, qvec) * invDet; 
    if (v < 0 || u + v > 1) return false; 
 
    float t = dot(v0v2, qvec) * invDet; 
 
    if (t > 0 && t < 1000000000){
         return true; 
    }
    return false;
}


#endif