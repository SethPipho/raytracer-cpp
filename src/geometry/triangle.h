#ifndef TRIANGLE_H
#define TRIANGLE_H

#include <iostream>
#include <string>
#include <vector>
#include <limits>
#include <random>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

#include "geometry/ray.h"
#include "geometry/mesh.h"
#include "geometry/bbox.h"

class Triangle{
    public:
        Mesh* mesh;
        int face_offset;
        Triangle(){};
        Triangle(Mesh* mesh, int face_offset): mesh(mesh), face_offset(face_offset){};
        Triangle(const Triangle& t): mesh(t.mesh), face_offset(t.face_offset){};
        glm::vec3 centroid();
        glm::vec3 flat_normal();
        glm::vec3 smooth_normal(glm::vec2 barycentric);
        glm::vec3 sample_point(float r1, float r2);    
        float area();
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

glm::vec3 Triangle::sample_point(float r1, float r2){
    glm::vec3 v0 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset));
    glm::vec3 v1 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset + 1));
    glm::vec3 v2 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset + 2));

    if (r1 + r2 > 1.f) {
        r1 = 1.f - r1;
        r2 = 1.f - r2;
    };
    return v0 + r1 * (v1 - v0) + r2 * (v2 - v0); 
};

float Triangle::area(){
    glm::vec3 v0 = this->mesh->vertices[this->mesh->face_indices[this->face_offset]];
    glm::vec3 v1 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 v2 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 2]];

    glm::vec3 e0 = v1 - v0;
    glm::vec3 e1 = v2 - v0;
    glm::vec3 n = glm::cross(e0, e1);
    return glm::length(n)/2.f;
};

glm::vec3 Triangle::smooth_normal(glm::vec2 barycentric){

    float u = barycentric.x;
    float v = barycentric.y;

    glm::vec3 n0 = this->mesh->normals[this->mesh->face_indices[this->face_offset]];
    glm::vec3 n1 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 n2 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 2]];

    return glm::normalize((n1 * u) + (n2 * v) + (n0 * (1 - u -v)));
}

glm::vec3 Triangle::flat_normal(){
    glm::vec3 v0 = this->mesh->vertices[this->mesh->face_indices[this->face_offset]];
    glm::vec3 v1 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 v2 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 2]];

    glm::vec3 e0 = v1 - v0;
    glm::vec3 e1 = v2 - v0;
    glm::vec3 n = glm::cross(e0, e1);
    return glm::normalize(n);
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


#endif