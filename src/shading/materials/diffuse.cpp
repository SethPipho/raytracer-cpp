#include "shading/materials/diffuse.h"
#include "util/math.h"

LambertianBSDF::LambertianBSDF(const glm::vec3 normal, const glm::vec3 albedo) {
    this->normal = normal;
    this->albedo = albedo;
    this->sample_light = true;
}

BSDFSample LambertianBSDF::sample(const glm::vec3& wo) {
    BSDFSample sample;
    glm::vec3 direction = sampleSphereUniform(randuf(), randuf());
    if (glm::dot(direction, normal) < 0.0f) {
        direction = -direction;
    }
    sample.throughput = eval(wo, direction);
    sample.direction = direction;
    sample.pdf = pdf(wo, direction);
    return sample;
}

glm::vec3 LambertianBSDF::eval(const glm::vec3& wo, const glm::vec3& wi) {
    float v = glm::dot(normal, wi);
    if (v < 0.0f) {
        return glm::vec3(0.0f);
    }
    return albedo/PI * v;
}

float LambertianBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi) {
    return 1.0f / PI;
}


DiffuseMaterial::DiffuseMaterial() {}

BSDF* DiffuseMaterial::create_shader(const IntersectionData& intersection) {
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
    BSDF* shader = new LambertianBSDF(normal, albedo);
    return shader;
}

