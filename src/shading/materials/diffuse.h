#ifndef DIFFUSE_H_
#define DIFFUSE_H_

#include "shading/material.h"
#include "shading/texture.h"


class DiffuseMaterial: public Material {
    public:
        glm::vec3 albedo = glm::vec3(.7f);
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        DiffuseMaterial();
        BSDF* create_shader(const IntersectionData& intersection) final;
};

class LambertianBSDF : public BSDF {
    public:
        glm::vec3 albedo;
        glm::vec3 normal;
        LambertianBSDF(const glm::vec3 normal, const glm::vec3 albedo);
        BSDFSample sample(const glm::vec3& wo) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi) final;
};
#endif