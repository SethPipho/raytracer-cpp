#ifndef MATERIAL_H_
#define MATERIAL_H_

#include "glm/glm.hpp"
#include "geometry/intersection.h"

struct BSDFSample {
    glm::vec3 direction;
    glm::vec3 throughput;
    glm::vec3 emission;
    float pdf;
};
 

class BSDF {
    public:
        bool sample_light;
        BSDF(){}
        virtual BSDFSample sample(const glm::vec3& wo) = 0;
        virtual glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) = 0;
        virtual float pdf(const glm::vec3& wo, const glm::vec3& wi) = 0;
};


class Material {
    public:
        bool emmissive = false;
        Material(){}
        virtual BSDF* create_shader(const IntersectionData& intersection) = 0;
};



/*
class ReflectionMaterial: public Material {
    public:
        glm::vec3 albedo = glm::vec3(1.0f);
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        ReflectionMaterial();
        BSDF* create_shader(const IntersectionData& intersection) final;
};

class EmissionMaterial: public Material {
    public:
        glm::vec3 emission = glm::vec3(1.0f);
        EmissionMaterial();
        EmissionMaterial(glm::vec3 emission);
        BSDF* create_shader(const IntersectionData& intersection) final;
};

*/

#endif