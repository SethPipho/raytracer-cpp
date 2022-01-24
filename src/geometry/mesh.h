#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <map>

#include "glm/glm.hpp"

class Material;


class Mesh {
    public:
        Material* material;
        bool is_light = false;
       
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> tex_coords;
        std::vector<glm::vec3> tangents;
        std::vector<glm::vec3> bitangents;
        std::vector<unsigned int> face_indices;
        Mesh(){};
        void compute_tangents();
        void applyTransform(glm::mat4 transform);
        static Mesh loadObj(std::string filename);
};


#endif