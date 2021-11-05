#ifndef MESH_H_
#define MESH_H_

#include <vector>

#include "glm/glm.hpp"

#define TINYOBJLOADER_IMPLEMENTATION 
#define TINYOBJLOADER_USE_FLOAT
#include "tinyobjloader/tiny_obj_loader.h"


class Mesh {
    public:
        //temp until material class
        glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
        glm::vec3 emission;
        bool is_light = false;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<int> face_indices;
        Mesh(){};
        void applyTransform(glm::mat4 transform);
        static Mesh loadObj(std::string filename);
};

void Mesh::applyTransform(glm::mat4 transform){
    glm::mat4 transform_normal = glm::inverse(glm::transpose(transform));
    for(int i = 0; i < vertices.size(); i++){
        this->vertices[i] = transform * glm::vec4(this->vertices[i], 1.0);
        this->normals[i] = transform_normal * glm::vec4(this->normals[i], 1.0);
    }
}

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
            glm::vec3 vertex = glm::vec3(vx, vy, vz);
            mesh.vertices.push_back(vertex);
            mesh.normals.push_back(glm::vec3(0.f, 0.f, 0.f));
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
                mesh.normals[idx.vertex_index] = glm::vec3(nx, ny, nz);
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

#endif