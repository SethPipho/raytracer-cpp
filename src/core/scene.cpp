#include <unordered_map>
#include <string>

#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE 
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON 
#include "tinygltf/tiny_gltf.h"
#include "json/json_fwd.hpp"

#include "core/scene.h"
#include "shading/material.h"
#include "shading/texture.h"
#include "shading/materials/all.h"
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


LightSample Scene::sampleLight(IntersectionData& intersection){
    LightSample sample;

    int index = randuf() * this->lights.size();
    Triangle light = this->lights.at(index);

    float u = randuf();
    float v = randuf();
    if (u + v > 1.f) {
        u = 1.f - u;
        v = 1.f - v;
    };
    glm::vec2 barycentric = glm::vec2(u,v);
    glm::vec3 position = light.position(barycentric);

    sample.light = light;
    sample.barycentric = barycentric;
    sample.position = position;
    sample.distance = glm::length(position - intersection.position);
    sample.direction = glm::normalize(position - intersection.position);
    sample.normal = light.normal(barycentric);
    sample.pdf = 1.0f / (this->lights.size() * light.area());

    return sample;
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

    
    std::unordered_map<std::string, Material*> material_map;
    for (auto mat_config: config["materials"]){
        std::string name = mat_config["name"];
        std::string type = mat_config["type"];


        if (type == "diffuse"){
            DiffuseMaterial* material = new DiffuseMaterial();
            if (mat_config.contains("albedo_texture")){
                std::string tex_path = dir + "/" + (std::string)mat_config["albedo_texture"];
                material->albedo_texture = TextureMap::load_file(tex_path);
            }
            material_map[name] = material;
        } else if (type == "reflection") {
            ReflectionMaterial* material = new ReflectionMaterial();
            if (mat_config.contains("albedo_texture")){
                std::string tex_path = dir + "/" + (std::string)mat_config["albedo_texture"];
                material->albedo_texture = TextureMap::load_file(tex_path);
            }
            material_map[name] = material;
        } else if (type == "emission") {
            EmissionMaterial* material = new EmissionMaterial();
            if (mat_config.contains("emission_color")){
                material->emission = vector_to_vec3(mat_config["emission_color"]);
            } 
            if (mat_config.contains("emission_strength")){
                material->emission *= mat_config["emission_strength"].get<float>();
            }
            material_map[name] = material;
        }
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
            DiffuseMaterial* material = new DiffuseMaterial();
            material->albedo = glm::vec3(0.8f);
            mesh.material = material;
            

            if (object.contains("material_ref")) {
                std::string material_name = object["material_ref"];
                mesh.material = material_map[material_name];
                mesh.is_light = mesh.material->emmissive;
            }
            
            if (object.count("light") > 0){
                mesh.is_light = object["light"].get<bool>();
            } 
        
            scene.meshes.push_back(mesh);
        }
    }
    return scene;
}


