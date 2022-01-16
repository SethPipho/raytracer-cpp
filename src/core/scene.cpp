#include <unordered_map>
#include <string>

#include "core/scene.h"

#include "json/json.hpp"

#include "materials/bsdf.h"
#include "materials/texture.h"

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
             material->use_texture = true;
        }
        material_map[name] = material;
    }

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

        if (object.contains("material_ref")){
            std::string material_name = object["material_ref"];
            mesh.bsdf = material_map[material_name];
        } else {
            mesh.bsdf = new LambertianBSDF();
            mesh.bsdf->albedo = glm::vec3(1.0f);
        }

     
        if (object.count("light") > 0){
            mesh.is_light = object["light"].get<bool>();
        } 
      
        scene.meshes.push_back(mesh);
    }
    return scene;
}