#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <map>

#include "glm/glm.hpp"

#define TINYOBJLOADER_IMPLEMENTATION 
#define TINYOBJLOADER_USE_FLOAT
#include "tinyobjloader/tiny_obj_loader.h"

#include "materials/bsdf.h"


class Mesh {
    public:
        //temp until material class

        BSDF* bsdf;
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

    //operator overload for std::map
    auto compare_idx = [](const tinyobj::index_t& a, const tinyobj::index_t& b) {
        return a.vertex_index < b.vertex_index ||
            (a.vertex_index == b.vertex_index && a.normal_index < b.normal_index) ||
            (a.vertex_index == b.vertex_index && a.normal_index == b.normal_index && a.texcoord_index < b.texcoord_index);
    };

     std::map<tinyobj::index_t, int,decltype(compare_idx)> unique_vertices(compare_idx);


    int current_idx;
     // copy face indices
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;
        
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];

                auto it = unique_vertices.find(idx);

                if (it != unique_vertices.end()) {
                    mesh.face_indices.push_back(it->second);
                } else {
                  
                    tinyobj::real_t vx = attrib.vertices[3*idx.vertex_index+0];
                    tinyobj::real_t vy = attrib.vertices[3*idx.vertex_index+1];
                    tinyobj::real_t vz = attrib.vertices[3*idx.vertex_index+2];
                    tinyobj::real_t nx = attrib.normals[3*idx.normal_index+0];
                    tinyobj::real_t ny = attrib.normals[3*idx.normal_index+1];
                    tinyobj::real_t nz = attrib.normals[3*idx.normal_index+2];
                    tinyobj::real_t tx = attrib.texcoords[2*idx.texcoord_index+0];
                    tinyobj::real_t ty = attrib.texcoords[2*idx.texcoord_index+1];

                    mesh.face_indices.push_back(current_idx);
                    mesh.vertices.push_back(glm::vec3(vx, vy, vz));
                    mesh.normals.push_back(glm::vec3(nx, ny, nz));

                    unique_vertices.insert(std::pair<tinyobj::index_t, int>(idx, current_idx));
                    current_idx += 1;
                }
            }
            index_offset += fv;

            // per-face material
            shapes[s].mesh.material_ids[f];
        }
    }
    return mesh;
}

#endif