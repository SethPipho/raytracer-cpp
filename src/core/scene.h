#ifndef SCENE_H_
#define SCENE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"

#include "geometry/geometry.h"
#include "core/bvh.h"
#include "core/camera.h"
#include "util/math.h"


using json = nlohmann::json;


struct LightSample {
    Triangle light;
    glm::vec2 barycentric;
    glm::vec3 position;
    glm::vec3 normal;
    float distance;
    glm::vec3 direction;
    float pdf;

    LightSample(){};
};

class Scene {
    public:
        std::vector<Mesh> meshes;
        std::vector<Triangle> triangles;
        std::vector<Triangle> lights;
        BVH bvh;
        Camera camera;
        
        Scene(){};
        void build();
        void addMesh(Mesh& mesh);
        Triangle& pickLight(float r);
        LightSample sampleLight(IntersectionData& intersection);
        IntersectionData nearestIntersection(Ray& r);
        static Scene load_file(std::string filepath);
};
#endif