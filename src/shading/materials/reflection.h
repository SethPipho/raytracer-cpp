#ifndef REFLECTION_H_
#define REFLECTION_H_

#include "shading/material.h"
#include "shading/texture.h"


class ReflectionBSDF : public BSDF {
    public:
        glm::vec3 albedo;
        glm::vec3 normal;
        ReflectionBSDF(const glm::vec3 normal, const glm::vec3 albedo);
        BSDFSample sample(const glm::vec3& wo) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi) final;
};


class ReflectionMaterial: public Material {
    public:
        glm::vec3 albedo = glm::vec3(1.0f);
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        ReflectionMaterial();
        BSDF* create_shader(const IntersectionData& intersection) final;
};

#endif