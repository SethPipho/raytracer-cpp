
#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <limits>

#include "glm/glm.hpp"

#include "geometry/triangle.h"
#include "geometry/bbox.h"
#include "geometry/ray.h"

#define TMAX 1.0e+10
#define TMIN 0.00001


/*
Class for holding data of ray-shape intersection 
*/
struct IntersectionData {
    bool hit = false;
    Triangle triangle;
    float t = std::numeric_limits<float>::max();
    glm::vec3 position;
    bool backface;

    //barycentric coordinates of hit, used to interpolating normals/uvs
    glm::vec2 barycentric;

    int depth;

    glm::vec3 face_normal;
    glm::vec3 smooth_normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;
    glm::vec2 tex_coord;
};

inline bool rayBBoxIntersection(const Ray& r, const BBox& box, float* t){

    float ep = 1.f + std::numeric_limits<float>::epsilon();

    glm::vec3 inverse_direction = 1.0f / r.direction;

    float tmin = (box.min.x - r.origin.x) * inverse_direction.x;
    float tmax = (box.max.x - r.origin.x) * inverse_direction.x;

    if (inverse_direction.x < 0.f) std::swap(tmin, tmax);

    tmax *= 1.00000024f;

    float tymin = (box.min.y - r.origin.y)  * inverse_direction.y;
    float tymax = (box.max.y - r.origin.y) * inverse_direction.y;

    if (inverse_direction.y < 0.f) std::swap(tymin, tymax);

    tymax *= 1.00000024f;

    if ((tmin > tymax) || (tymin > tmax)) return false;
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;
    
    float tzmin = (box.min.z - r.origin.z) * inverse_direction.z;
    float tzmax = (box.max.z - r.origin.z) * inverse_direction.z;
    if (inverse_direction.z < 0.f) std::swap(tzmin, tzmax);

    tzmax *= 1.00000024f;

    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;
 
    
    *t = tmin;
   
    
    return true;
}

//Möller–Trumbore intersection algorithm
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
inline IntersectionData rayTriangleIntersection(Ray &ray, Triangle &triangle){

    IntersectionData intersection;

    int face_offset = triangle.face_offset;
    Mesh* mesh = triangle.mesh;
    
    glm::vec3 v0 = mesh->vertices[mesh->face_indices[face_offset]];
    glm::vec3 v1 = mesh->vertices[mesh->face_indices[face_offset + 1]];
    glm::vec3 v2 = mesh->vertices[mesh->face_indices[face_offset + 2]];

    glm::vec3 v0v1 = v1 - v0; 
    glm::vec3 v0v2 = v2 - v0; 
    glm::vec3 face_normal = glm::normalize(glm::cross(v0v1, v0v2));
    glm::vec3 pvec = glm::cross(ray.direction, v0v2);
    float det = glm::dot(v0v1, pvec); 

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 0.0) return intersection; 

    float invDet = 1 / det; 
    glm::vec3 tvec = ray.origin - v0; 
    float u = glm::dot(tvec, pvec) * invDet; 
    if (u <= -0.000001f || u >= 1.000001f) return intersection; 
 
    glm::vec3 qvec = glm::cross(tvec, v0v1); 
    float v = glm::dot(ray.direction, qvec) * invDet; 
    if (v <= -0.000001f || u + v >= 1.000001f) return intersection; 
 
    float t = dot(v0v2, qvec) * invDet; 
    
 
    if (t <= TMIN || t >= TMAX){
        return intersection;
    }
    
    intersection.hit = true;
    intersection.t = t;
    intersection.position = ray.origin + ray.direction * t;
    intersection.triangle = triangle;
    intersection.barycentric = glm::vec2(u, v);
    intersection.backface  = (glm::dot(ray.direction, face_normal) >= 0);
    intersection.face_normal = face_normal;
    return intersection; 
}


#endif