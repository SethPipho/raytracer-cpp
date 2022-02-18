#include "shading/materials/reflection.h"


ReflectionBSDF::ReflectionBSDF(const glm::vec3 normal, const glm::vec3 albedo) {
    this->normal = normal;
    this->albedo = albedo;
    this->sample_light = false;
}

BSDFSample ReflectionBSDF::sample(const glm::vec3& wo) {
    BSDFSample sample;
    glm::vec3 direction = glm::reflect(wo, normal);
    sample.throughput = eval(wo, direction);
    sample.direction = direction;
    sample.pdf = 1.0;
    return sample;
}

glm::vec3 ReflectionBSDF::eval(const glm::vec3& wo, const glm::vec3& wi) {
    return albedo;
}

float ReflectionBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi) {
    return 0.0f;
}



ReflectionMaterial::ReflectionMaterial() {}

BSDF* ReflectionMaterial::create_shader(const IntersectionData& intersection) {
    glm::vec3 albedo = this->albedo;
    if (albedo_texture != nullptr) {
        albedo = albedo_texture->sample(intersection.tex_coord);
    }

    glm::vec3 normal = intersection.normal;
    if (normal_texture != nullptr) {
        glm::vec3 tex_normal = normal_texture->sample(intersection.tex_coord) * 2.0f - 1.0f;
        glm::mat3 tangent_space_to_world = glm::mat3(intersection.tangent, intersection.bitangent, intersection.normal);
        normal = glm::normalize(tangent_space_to_world * tex_normal);
    }
    BSDF* shader = new ReflectionBSDF(normal, albedo);
    return shader;
}
