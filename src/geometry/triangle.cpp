
#include "geometry/triangle.h"

glm::vec3 Triangle::centroid(){
    glm::vec3 v0 = this->mesh->vertices[this->mesh->face_indices[this->face_offset]];
    glm::vec3 v1 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 v2 = this->mesh->vertices[this->mesh->face_indices[this->face_offset + 2]];

    float x = (v0.x + v1.x + v2.x)/3.f;
    float y = (v0.y + v1.y + v2.y)/3.f;
    float z = (v0.z + v1.z + v2.z)/3.f;
    return glm::vec3(x,y,z);
};

glm::vec3 Triangle::position(glm::vec2 barycentric){
    glm::vec3 v0 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset));
    glm::vec3 v1 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset + 1));
    glm::vec3 v2 = this->mesh->vertices.at(this->mesh->face_indices.at(this->face_offset + 2));

    return v0 + barycentric.x * (v1 - v0) + barycentric.y * (v2 - v0); 
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

glm::vec3 Triangle::normal(glm::vec2 barycentric){

    float u = barycentric.x;
    float v = barycentric.y;

    glm::vec3 n0 = this->mesh->normals[this->mesh->face_indices[this->face_offset]];
    glm::vec3 n1 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 n2 = this->mesh->normals[this->mesh->face_indices[this->face_offset + 2]];

    return glm::normalize((n1 * u) + (n2 * v) + (n0 * (1 - u -v)));
}

glm::vec3 Triangle::tangent(glm::vec2 barycentric){

    float u = barycentric.x;
    float v = barycentric.y;

    glm::vec3 t0 = this->mesh->tangents[this->mesh->face_indices[this->face_offset]];
    glm::vec3 t1 = this->mesh->tangents[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec3 t2 = this->mesh->tangents[this->mesh->face_indices[this->face_offset + 2]];

    return glm::normalize((t1 * u) + (t2 * v) + (t0 * (1 - u -v)));
}

glm::vec3 Triangle::bitangent(glm::vec2 barycentric){
        float u = barycentric.x;
        float v = barycentric.y;
    
        glm::vec3 b0 = this->mesh->bitangents[this->mesh->face_indices[this->face_offset]];
        glm::vec3 b1 = this->mesh->bitangents[this->mesh->face_indices[this->face_offset + 1]];
        glm::vec3 b2 = this->mesh->bitangents[this->mesh->face_indices[this->face_offset + 2]];
    
        return glm::normalize((b1 * u) + (b2 * v) + (b0 * (1 - u -v)));
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

glm::vec2 Triangle::tex_coords(glm::vec2 barycentric){
    float u = barycentric.x;
    float v = barycentric.y;

    glm::vec2 t0 = this->mesh->tex_coords[this->mesh->face_indices[this->face_offset]];
    glm::vec2 t1 = this->mesh->tex_coords[this->mesh->face_indices[this->face_offset + 1]];
    glm::vec2 t2 = this->mesh->tex_coords[this->mesh->face_indices[this->face_offset + 2]];

    return t1 * u + t2 * v + t0 * (1 - u - v);
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

