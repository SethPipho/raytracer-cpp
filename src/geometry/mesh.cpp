#define TINYOBJLOADER_IMPLEMENTATION 
#define TINYOBJLOADER_USE_FLOAT
#include "tinyobjloader/tiny_obj_loader.h"

#include "geometry/mesh.h"

#include <iostream>

void Mesh::compute_tangents(){
    tangents.resize(vertices.size());
    bitangents.resize(vertices.size());
    for (unsigned int i = 0; i < vertices.size(); i++){
        tangents[i] = glm::vec3(0.0f);
        bitangents[i] = glm::vec3(0.0f);
    }
    //http://foundationsofgameenginedev.com/FGED2-sample.pdf
    for (unsigned int i = 0; i < face_indices.size(); i += 3) {
        glm::vec3 p0 = vertices[face_indices[i]];
        glm::vec3 p1 = vertices[face_indices[i+1]];
        glm::vec3 p2 = vertices[face_indices[i+2]];
        
        glm::vec2 w0 = tex_coords[face_indices[i]];
        glm::vec2 w1 = tex_coords[face_indices[i+1]];
        glm::vec2 w2 = tex_coords[face_indices[i+2]];

        glm::vec3 e1 = p1 - p0;
        glm::vec3 e2 = p2 - p0;

        float x1 = w1.x - w0.x;
        float x2 = w2.x - w0.x;
        float y1 = w1.y - w0.y;
        float y2 = w2.y - w0.y;

    
        float r = 1.0f/(x1 * y2 - x2 * y1);
        glm::vec3 tangent = (e1 * y2 - e2 * y1) * r;
        glm::vec3 bitangent = (e2 * x1 - e1 * x2) * r;


        tangents[face_indices[i]] += tangent;
        tangents[face_indices[i+1]] += tangent;
        tangents[face_indices[i+2]] += tangent;

        bitangents[face_indices[i]] += bitangent;
        bitangents[face_indices[i+1]] += bitangent;
        bitangents[face_indices[i+2]] += bitangent;
    }
     for(int i = 0; i < vertices.size(); i++){
        tangents[i] = glm::normalize(tangents[i]);
        bitangents[i] = glm::normalize(bitangents[i]);

        if (glm::dot(glm::cross(tangents[i], bitangents[i]), normals[i]) < 0.0f) {
           // bitangents[i] = -bitangents[i];
            
        }
     
       
     }
   
}


void Mesh::applyTransform(glm::mat4 transform){
    glm::mat4 transform_normal = glm::inverse(glm::transpose(transform));
    for(int i = 0; i < vertices.size(); i++){
        this->vertices[i] = transform * glm::vec4(this->vertices[i], 1.0);
        this->tangents[i] = glm::normalize(transform * glm::vec4(this->tangents[i], 0.0));
        this->bitangents[i] = glm::normalize(transform_normal * glm::vec4(this->bitangents[i], 0.0));
        this->normals[i] = glm::normalize(transform_normal * glm::vec4(this->normals[i], 0.0));
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

    bool has_tex_coords = attrib.texcoords.size() > 0;
    int current_idx = 0;
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

                    tinyobj::real_t tx, ty;
                    if (has_tex_coords){
                        tx = attrib.texcoords[2*idx.texcoord_index+0];
                        ty = attrib.texcoords[2*idx.texcoord_index+1];
                    } else {
                        tx = 0.0f;
                        ty = 0.0f;
                    }
                 

                    mesh.face_indices.push_back(current_idx);
                    mesh.vertices.push_back(glm::vec3(vx, vy, vz));
                    mesh.normals.push_back(glm::vec3(nx, ny, nz));
                    mesh.tex_coords.push_back(glm::vec2(tx, ty));

                    unique_vertices.insert(std::pair<tinyobj::index_t, int>(idx, current_idx));
                    current_idx += 1;
                }
            }
            index_offset += fv;
        }
    }
    mesh.compute_tangents();
    return mesh;
}