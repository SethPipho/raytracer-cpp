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
        glm::vec3 normal(glm::vec2 barycentric);
        glm::vec3 tangent(glm::vec2 barycentric);
        glm::vec3 bitangent(glm::vec2 barycentric);
        glm::vec2 tex_coords(glm::vec2 barycentric);
        glm::vec3 position(glm::vec2 barycentric);    
        float area();
        BBox bbox();
};

#endif