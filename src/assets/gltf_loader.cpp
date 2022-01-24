#include <iostream>
#include "assets/gtlf_loader.h"
#include "materials/material.h"
#include "materials/texture.h"

TextureMap* load_gltf_image(tinygltf::Image &gltf_image, bool convert_linear){
    TextureMap* texture_map = new TextureMap();

    texture_map->w = gltf_image.width;
    texture_map->h = gltf_image.height;
    texture_map->n = gltf_image.component;
    texture_map->data = new glm::vec3[gltf_image.width * gltf_image.height];
    auto image_data = gltf_image.image.data();

    for (int i = 0; i < gltf_image.width * gltf_image.height; i++){
        float r = image_data[i * gltf_image.component + 0]/255.0f;
        float g = image_data[i * gltf_image.component + 1]/255.0f;
        float b = image_data[i * gltf_image.component + 2]/255.0f;
        glm::vec3 color(r, g, b);
        if (convert_linear){
            color = glm::pow(color, glm::vec3(2.2f)); // linear space
        }
        texture_map->data[i] = color;
    }
    return texture_map;
}


//based on https://github.com/syoyo/tinygltf/blob/master/examples/raytrace/gltf-loader.cc
std::vector<Mesh> load_gltf(std::string filepath){
    auto meshes = std::vector<Mesh>();

    std::cout << "loading gltf file " << filepath << std::endl;

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, filepath);
    //bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, argv[1]); // for binary glTF(.glb)
    if (!warn.empty()) {
        std::cout << "glTF parse warning: " << warn << std::endl;
    }

    if (!err.empty()) {
        std::cerr << "glTF parse error: " << err << std::endl;
    }
    if (!ret) {
        std::cerr << "Failed to load glTF: " << filepath << std::endl;
    }

    /*
    std::cout << "loaded glTF file has:\n"
                << model.accessors.size() << " accessors\n"
                << model.animations.size() << " animations\n"
                << model.buffers.size() << " buffers\n"
                << model.bufferViews.size() << " bufferViews\n"
                << model.materials.size() << " materials\n"
                << model.meshes.size()   << " meshes\n"
                << model.nodes.size()    << " nodes\n"
                << model.textures.size() << " textures\n"
                << model.images.size()   << " images\n"
                << model.skins.size()    << " skins\n"
                << model.samplers.size() << " samplers\n"
                << model.cameras.size()  << " cameras\n"
                << model.scenes.size()   << " scenes\n"
                << model.lights.size()   << " lights\n";
    */

    auto materials = std::vector<Material*>();

    for (const auto gltf_material: model.materials){

        auto material = new DiffuseMaterial();
        material->albedo = glm::vec3(0.8f);
      
        auto &pbr_metallic_roughness = gltf_material.pbrMetallicRoughness;
        auto &base_color_texture_info = pbr_metallic_roughness.baseColorTexture;

        if (base_color_texture_info.index != -1){
            auto &gltf_texture = model.textures[base_color_texture_info.index];
            auto &gltf_image = model.images[gltf_texture.source];

            auto texture_map = load_gltf_image(gltf_image, true);
            material->albedo_texture = texture_map;
        }

        //normal map
        auto &normal_texture_info = gltf_material.normalTexture;
        if (normal_texture_info.index != -1){
            auto &gltf_texture = model.textures[normal_texture_info.index];
            auto &gltf_image = model.images[gltf_texture.source];

            auto texture_map = load_gltf_image(gltf_image, false);
            material->normal_texture = texture_map;
        }
        materials.push_back(material);
    }


    for (const auto &gltfMesh : model.meshes) {
        for (const auto &meshPrimitive : gltfMesh.primitives) {

            Mesh mesh;

            mesh.material = materials[meshPrimitive.material];
            
            //mesh indices
            const auto &indicesAccessor = model.accessors[meshPrimitive.indices];
            const auto &bufferView = model.bufferViews[indicesAccessor.bufferView];
            const auto &buffer = model.buffers[bufferView.buffer];
            const auto dataAddress = buffer.data.data() + bufferView.byteOffset + indicesAccessor.byteOffset;
            const auto byteStride = indicesAccessor.ByteStride(bufferView);
            const auto count = indicesAccessor.count;

            IntArrayAdapterBase* indices;

            switch (indicesAccessor.componentType) {
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    indices = new IntArrayAdapter<unsigned short>(dataAddress, byteStride, count);
                    break;
                break;
                case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
                    indices = new IntArrayAdapter<unsigned int>(dataAddress, byteStride, count);
                    break;
                break;
                default:
                    std::cout << "unsupported indices type" << indicesAccessor.componentType << std::endl;
                    return meshes;
                break;

            }

            for (int i = 0; i < count; i++) {
                mesh.face_indices.push_back(indices->get(i));
            }

            //mesh attributes
            for (const auto &attribute : meshPrimitive.attributes) {
                
                const auto attribAccessor = model.accessors[attribute.second];
                const auto &bufferView = model.bufferViews[attribAccessor.bufferView];
                const auto &buffer = model.buffers[bufferView.buffer];
                const auto dataPtr = buffer.data.data() + bufferView.byteOffset + attribAccessor.byteOffset;
                const auto byte_stride = attribAccessor.ByteStride(bufferView);
                const auto count = attribAccessor.count;

                if  (attribute.first == "POSITION") {
                    Vec3ArrayAdapter *positions = new Vec3ArrayAdapter(dataPtr, byte_stride, count);
                    for (int i = 0; i < count; i++) {
                        mesh.vertices.push_back(positions->get(i));
                    }
                } else if  (attribute.first == "NORMAL") {
                    Vec3ArrayAdapter *normals = new Vec3ArrayAdapter(dataPtr, byte_stride, count);
                    for (int i = 0; i < count; i++) {
                        mesh.normals.push_back(normals->get(i));
                    }
                } else if (attribute.first == "TEXCOORD_0") {
                    Vec2ArrayAdapter *tex_coords = new Vec2ArrayAdapter(dataPtr, byte_stride, count);
                    for (int i = 0; i < count; i++) {
                        auto uv = tex_coords->get(i);
                        uv.y = 1.0f - uv.y;
                        mesh.tex_coords.push_back(uv);
                    }
                }
            }
            mesh.compute_tangents();
            meshes.push_back(mesh);
        }
    }
    return meshes;
}