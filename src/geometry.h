#ifndef GEOMETERY_H_
#define GEOMETERY_H_

#include <iostream>
#include <string>
#include <vector>

#include "glm/glm.hpp"



#include "ray.h"
#include "mesh.h"

#define TMAX 1.0e+10
#define TMIN 0.0000000000001

class BBox {
    public:
        glm::vec3 min, max;
        BBox(){}
        BBox(glm::vec3 min, glm::vec3 max){
            this->min = min;
            this->max = max;
        }
        glm::vec3 centroid();
        static BBox unionBBox(const BBox& a, const BBox& n);
};

glm::vec3 BBox::centroid(){
    return this->min + ((this->max - this->min)/2.0f);
};

BBox BBox::unionBBox(const BBox& a, const BBox& b){
    glm::vec3 minb(std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y), std::min(a.min.z, b.min.z));
    glm::vec3 maxb(std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y), std::max(a.max.z, b.max.z));
    return BBox(minb, maxb);
};

class Triangle{
    public:
        Mesh* mesh;
        int face_offset;
        Triangle(){};
        Triangle(Mesh* mesh, int face_offset): mesh(mesh), face_offset(face_offset){};
        Triangle(const Triangle& t): mesh(t.mesh), face_offset(t.face_offset){};
        glm::vec3 centroid();
};

glm::vec3 Triangle::centroid(){
    glm::vec3 v0 = this->mesh->vertices[this->mesh->face_indices[this->face_offset]];
    glm::vec3 v1 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 v2 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 2]];

    float x = (v0.x + v1.x + v2.x)/3.f;
    float y = (v0.y + v1.y + v2.y)/3.f;
    float z = (v0.z + v1.z + v2.z)/3.f;
    return glm::vec3(x,y,z);
};


/*
Class for holding data of ray-shape intersection 
*/
class IntersectionData {
    public:
        glm::vec3 normal;
        float t;
        glm::vec3 hit;
        bool backface;
        Triangle triangle;
        IntersectionData(){t = TMAX;};
};


inline bool rayBBoxIntersection(const Ray& r, const BBox& box, double tmin, double tmax, double* t){

    double t1 = (box.min.x - r.origin.x) / r.direction.x;
    double t2 = (box.max.x - r.origin.x) / r.direction.x;
    
    if (t1 > t2) std::swap(t1, t2);
    
    double tymin = (box.min.y - r.origin.y) / r.direction.y;
    double tymax = (box.max.y - r.origin.y) / r.direction.y;

    if (tymin > tymax) std::swap(tymin, tymax);
    if ((t1 > tymax) || (t2 < tymin)) return false;
    if (tymin > t1) t1 = tymin;
    if (tymax < t2) t2 = tymax;
    double tzmin = (box.min.z - r.origin.z) / r.direction.z;
    double tzmax = (box.max.z - r.origin.z) / r.direction.z;
    if (tzmin > tzmax) std::swap(tzmin, tzmax);
    if ((t1 > tzmax) || (t2 < tzmin)) return false;
    if (tzmin > t1) t1 = tzmin;
    if (tzmax < t2) t2 = tzmax;
    if (t1 > t2) std::swap(t1,t2);
    if ((t1 > tmin) && (t1 < tmax)){
        *t = t1;
        return true;
    }
   if ((t2 > tmin) && (t2 < tmax)){
        *t = t2;
        return true;
    }
    return false;
}

//Möller–Trumbore intersection algorithm
//https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
inline bool rayTriangleIntersection(Ray &ray,Triangle &triangle, float tmin, float tmax, IntersectionData* intersection){

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
    if (fabs(det) < 0.000001) return false; 

    float invDet = 1 / det; 
 
    glm::vec3 tvec = ray.origin - v0; 
    float u = glm::dot(tvec, pvec) * invDet; 
    if (u < 0 || u > 1) return false; 
 
    glm::vec3 qvec = glm::cross(tvec, v0v1); 
    float v = glm::dot(ray.direction, qvec) * invDet; 
    if (v < 0 || u + v > 1) return false; 
 
    float t = dot(v0v2, qvec) * invDet; 
 
    if (t > tmin && t < tmax){

        glm::vec3 n0 = mesh->normals[mesh->face_indices[face_offset]];
        glm::vec3 n1 = mesh->normals[mesh->face_indices[face_offset + 1]];
        glm::vec3 n2 = mesh->normals[mesh->face_indices[face_offset + 2]];

        glm::vec3 normal = glm::normalize((n1 * u) + (n2 * v) + (n0 * (1 - u -v)));
        intersection->t = t;
        intersection->hit = ray.origin + ray.direction * t;
        intersection->triangle = triangle;
      
        if (glm::dot(ray.direction, face_normal) <= 0){
            intersection->normal = normal;
            intersection->backface = false;
        } else {
            intersection->normal = -normal;
            intersection->backface = true;
        }
         return true; 
    }
    return false;
}


#endif