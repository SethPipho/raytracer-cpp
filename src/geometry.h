#ifndef GEOMETERY_H_
#define GEOMETERY_H_

#include <iostream>
#include <string>
#include <vector>
#include <limits>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "ray.h"
#include "mesh.h"

#define TMAX 1.0e+10
#define TMIN 0.001

class BBox {
    public:
        glm::vec3 min, max;
        BBox(){
            this->min = glm::vec3(std::numeric_limits<float>::infinity());
            this->max = glm::vec3(-std::numeric_limits<float>::infinity());

        }
        BBox(glm::vec3 min, glm::vec3 max){
            this->min = min;
            this->max = max;
        }
        BBox(const BBox& other){
            this->min = other.min;
            this->max = other.max;
        }
        glm::vec3 centroid();
        float surface_area();
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

float BBox::surface_area(){
    glm::vec3 l = this->max - this->min;
    return (2 * l[0] * l[1]) + (2 * l[0] * l[2]) + (2 * l[1] * l[2]);
};

std::ostream& operator << ( std::ostream& outs, const BBox& box ){
  return outs << "BBox(" << glm::to_string(box.min) << "," << glm::to_string(box.max) << ")";
}

class Triangle{
    public:
        Mesh* mesh;
        int face_offset;
        Triangle(){};
        Triangle(Mesh* mesh, int face_offset): mesh(mesh), face_offset(face_offset){};
        Triangle(const Triangle& t): mesh(t.mesh), face_offset(t.face_offset){};
        glm::vec3 centroid();
        glm::vec3 smooth_normal(float u, float v);
        BBox bbox();
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

glm::vec3 Triangle::smooth_normal(float u, float v){
    glm::vec3 n0 = this->mesh->normals[this->mesh->face_indices[this->face_offset]];
    glm::vec3 n1 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 n2 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 2]];

    return glm::normalize((n1 * u) + (n2 * v) + (n0 * (1 - u -v)));
}


BBox Triangle::bbox(){
    glm::vec3 v0 = this->mesh->vertices[this->mesh->face_indices[this->face_offset]];
    glm::vec3 v1 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 v2 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 2]];

    float minx = std::min(std::min(v0.x, v1.x), v2.x); 
    float miny = std::min(std::min(v0.y, v1.y), v2.y); 
    float minz = std::min(std::min(v0.z, v1.z), v2.z); 

    float maxx = std::max(std::max(v0.x, v1.x), v2.x); 
    float maxy = std::max(std::max(v0.y, v1.y), v2.y); 
    float maxz = std::max(std::max(v0.z, v1.z), v2.z); 

    return BBox(glm::vec3(minx, miny, minz), glm::vec3(maxx, maxy, maxz));
}


/*
Class for holding data of ray-shape intersection 
*/
struct IntersectionData {
    Triangle triangle;
    float t = TMAX;
    glm::vec3 position;
    bool backface;

    //barycentric coordinates of hit, used to interpolating normals/uvs
    float bu;
    float bv;

    int depth;

    glm::vec3 face_normal;
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
    
    intersection.t = t;
    intersection.position = ray.origin + ray.direction * t;
    intersection.triangle = triangle;
    intersection.bu = u;
    intersection.bv = v;
    intersection.backface  = (glm::dot(ray.direction, face_normal) >= 0);
    intersection.face_normal = face_normal;
    return intersection; 
}


#endif