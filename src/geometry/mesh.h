#ifndef MESH_H_
#define MESH_H_

#include <vector>
#include <map>

#include "glm/glm.hpp"

class BSDF;


class Mesh {
    public:
        //temp until material class

        BSDF* bsdf;
        bool is_light = false;
        
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> tex_coords;
        std::vector<int> face_indices;
        Mesh(){};
        void applyTransform(glm::mat4 transform);
        static Mesh loadObj(std::string filename);
};


#endif