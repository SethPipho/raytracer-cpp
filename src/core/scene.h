#ifndef SCENE_H_
#define SCENE_H_

#include <iostream>
#include <vector>
#include <string>

#include "glm/glm.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "json/json.hpp"

#include "geometry/geometry.h"
#include "core/bvh.h"
#include "core/camera.h"
#include "util/math.h"
#include "materials/bsdf.h"

using json = nlohmann::json;

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
        IntersectionData nearestIntersection(Ray& r);
        static Scene load_file(std::string filepath);
};

void Scene::build(){
    std::cout << "building scene" << std::endl;
    this->triangles.clear();
    for (Mesh& mesh: this->meshes){
        this->addMesh(mesh);
    } 
    bvh = BVH(&this->triangles);
    bvh.build();
}

Triangle& Scene::pickLight(float r){
    int index = r * this->lights.size();
    return this->lights.at(index);
}

void Scene::addMesh(Mesh& mesh){
     for (int i = 0; i < mesh.face_indices.size(); i+=3){
        Triangle t(&mesh, i);
        this->triangles.push_back(t);
        if (mesh.is_light){
            this->lights.push_back(t);
        }
    } 
};


Scene Scene::load_file(std::string filepath){
    std::cout << "loading scene" << std::endl;

    Scene scene;

    std::ifstream file(filepath);
    json config;
    file >> config;

    std::string dir = filepath.substr(0, filepath.find_last_of("\\/"));

    json camera_config = config["camera"];
    scene.camera = Camera::from_json(camera_config);

    for (auto object: config["objects"]){
        std::string mesh_path = dir + "/" + (std::string) object["path"];
        Mesh mesh = Mesh::loadObj(mesh_path);

        glm::vec3 position = vector_to_vec3(object["transform"]["position"]);
        glm::vec3 rotation = vector_to_vec3(object["transform"]["rotation"]);
        glm::vec3 scale =  vector_to_vec3(object["transform"]["scale"]);

        glm::mat4 translate_m = glm::translate(position);
        glm::mat4 rotation_m = glm::eulerAngleYXZ(glm::radians(rotation[1]), glm::radians(rotation[0]), glm::radians(rotation[2]));
        glm::mat4 scale_m = glm::scale(scale);
        glm::mat4 transform = translate_m * rotation_m * scale_m;
    
        mesh.applyTransform(transform);

        mesh.bsdf = new LambertianBSDF();

        if (object.count("light") > 0){
            mesh.is_light = object["light"].get<bool>();
        } 
        if (object.count("color") > 0){
            mesh.bsdf->albedo = vector_to_vec3(object["color"].get<std::vector<float>>());
        } else {
            mesh.bsdf->albedo = glm::vec3(1.0f);
        }
        scene.meshes.push_back(mesh);
    }
    return scene;
}
#endif