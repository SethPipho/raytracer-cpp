#include <unordered_map>
#include <string>

#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE 
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON 
#include "tinygltf/tiny_gltf.h"
#include "json/json_fwd.hpp"

#include "core/scene.h"
#include "materials/bsdf.h"
#include "materials/texture.h"
#include "assets/gtlf_loader.h"

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


    std::unordered_map<std::string, BSDF*> material_map;
    for (auto mat_config: config["materials"]){
        std::string name = mat_config["name"];
        std::string type = mat_config["type"];
        
        LambertianBSDF* material = new LambertianBSDF();

        if (mat_config.contains("albedo_texture")){
             std::string tex_path = dir + "/" + (std::string)mat_config["albedo_texture"];
             material->albedo_texture = TextureMap::load_file(tex_path);
        }
        material_map[name] = material;
    }

    for (auto object: config["objects"]){
        std::string mesh_path = dir + "/" + (std::string) object["path"];
        
        glm::vec3 position = vector_to_vec3(object["transform"]["position"]);
        glm::vec3 rotation = vector_to_vec3(object["transform"]["rotation"]);
        float scale =  object["transform"]["scale"].get<float>();

        glm::mat4 translate_m = glm::translate(position);
        glm::mat4 rotation_m = glm::eulerAngleYXZ(glm::radians(rotation[1]), glm::radians(rotation[0]), glm::radians(rotation[2]));
        glm::mat4 scale_m = glm::scale(glm::vec3(scale));
        glm::mat4 transform = translate_m * rotation_m * scale_m;
    

    
        if (object["type"] == "gltf"){
            auto meshes = load_gltf(mesh_path);
            for (auto mesh: meshes){
                mesh.applyTransform(transform);
                scene.meshes.push_back(mesh);
            }
        } else {

            Mesh mesh = Mesh::loadObj(mesh_path);
            mesh.applyTransform(transform);

            //default material
            mesh.bsdf = new LambertianBSDF();
            mesh.bsdf->albedo = glm::vec3(1.0f);

            if (object.contains("material_ref")){
                std::string material_name = object["material_ref"];
                mesh.bsdf = material_map[material_name];
            }
        
            if (object.count("light") > 0){
                mesh.is_light = object["light"].get<bool>();
            } 
        
            scene.meshes.push_back(mesh);
        }
    }
    return scene;
}


