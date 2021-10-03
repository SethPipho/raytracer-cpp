#ifndef GEOMETERY_H_
#define GEOMETERY_H_

#include <iostream>
#include <string>
#include <vector>

#include "glm/glm.hpp"
#define TINYOBJLOADER_IMPLEMENTATION 
#define TINYOBJLOADER_USE_FLOAT
#include "tinyobjloader/tiny_obj_loader.h"


#include "ray.h"

#define TMAX 1.0e+10
#define TMIN 0.0000000000001

class Mesh {
    public:
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<int> face_indices;
        std::vector<int> normal_indices;
       
        Mesh(){};
        static Mesh loadObj(std::string filename);
};

Mesh Mesh::loadObj(std::string filename){
    Mesh mesh;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string err;

    bool ret = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str());

    
    //copy vertices into mesh
    for (int i = 0; i < attrib.vertices.size(); i+=3){
            float vx = attrib.vertices[i];
            float vy = attrib.vertices[i + 1];
            float vz = attrib.vertices[i + 2];
            float nx = attrib.normals[i];
            float ny = attrib.normals[i + 1];
            float nz = attrib.normals[i + 2];
            glm::vec3 vertex = glm::vec3(vx, vy, vz) + glm::vec3(3.f, 0.f, 0.f);
            glm::vec3 normal = glm::vec3(nx, ny, nz);
            mesh.vertices.push_back(vertex);
            mesh.normals.push_back(normal);
    }

     // copy face indices
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

              
                mesh.face_indices.push_back(idx.vertex_index);
                mesh.normal_indices.push_back(idx.normal_index);
                // Optional: vertex colors
                // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    return mesh;
}



class Triangle{
    public:
        Mesh* mesh;
        int face_offset;
        Triangle(){};
        Triangle(Mesh* mesh, int face_offset): mesh(mesh), face_offset(face_offset){};
        Triangle(const Triangle& t): mesh(t.mesh), face_offset(t.face_offset){};
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

        glm::vec3 n0 = mesh->normals[mesh->normal_indices[face_offset]];
        glm::vec3 n1 = mesh->normals[mesh->normal_indices[face_offset + 1]];
        glm::vec3 n2 = mesh->normals[mesh->normal_indices[face_offset + 2]];

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