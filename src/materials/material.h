#ifndef BSDF_H_
#define BSDF_H_

#include "glm/glm.hpp"

#include "geometry/intersection.h"
#include "util/math.h"
#include "materials/texture.h"

struct BSDFSample {
    glm::vec3 direction;
    glm::vec3 throughput;
    glm::vec3 emission;
    float pdf;
};
 

class BSDF {
    public:
        glm::vec3 emission;
        glm::vec3 albedo = glm::vec3(.7f);
        bool is_light = false;
        bool sample_light;
        BSDF(){}
        virtual BSDFSample sample(const glm::vec3& wo) = 0;
        virtual glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) = 0;
        virtual float pdf(const glm::vec3& wo, const glm::vec3& wi) = 0;
};


class Material {
    public:
        Material(){}
        virtual BSDF* create_shader(const IntersectionData& intersection) = 0;
};


class LambertianBSDF : public BSDF {
    public:
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        glm::vec3 albedo;
        glm::vec3 normal;
        LambertianBSDF(const glm::vec3 normal, const glm::vec3 albedo): normal(normal), albedo(albedo) {
            this->sample_light = true;
        }
        BSDFSample sample(const glm::vec3& wo) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi) final;
};

inline BSDFSample LambertianBSDF::sample(const glm::vec3& wo) {
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

inline glm::vec3 LambertianBSDF::eval(const glm::vec3& wo, const glm::vec3& wi) {

    float v = glm::dot(normal, wi);
    if (v < 0.0f) {
        return glm::vec3(0.0f);
    }
    return albedo/PI * v;
}

inline float LambertianBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi) {
    return 1.0f / PI;
}


class ReflectionBSDF: public BSDF {
    public:
        glm::vec3 albedo;
        glm::vec3 normal;
        ReflectionBSDF(const glm::vec3 normal, const glm::vec3 albedo): normal(normal), albedo(albedo) {
            this->sample_light = false;
        }
        BSDFSample sample(const glm::vec3& wo) final {
            BSDFSample sample;
            glm::vec3 direction = glm::reflect(-wo, normal);
            sample.throughput = eval(wo, direction);
            sample.direction = direction;
            sample.pdf = pdf(wo, direction);
            return sample;
        };
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi) final{
            return glm::vec3(1.0f)/PI;
        };
        float pdf(const glm::vec3& wo, const glm::vec3& wi) final {
            return 1.0f;
        };
};

class DiffuseMaterial: public Material {
    public:
        glm::vec3 albedo = glm::vec3(.7f);
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        DiffuseMaterial(){};
        BSDF* create_shader(const IntersectionData& intersection) final {

            glm::vec3 albedo = this->albedo;
            if (albedo_texture != nullptr) {
                albedo = albedo_texture->sample(intersection.tex_coord);
            }

            glm::vec3 normal = intersection.smooth_normal;
            if (normal_texture != nullptr) {
                glm::vec3 tex_normal = normal_texture->sample(intersection.tex_coord) * 2.0f - 1.0f;
                glm::mat3 tangent_space_to_world = glm::mat3(intersection.tangent, intersection.bitangent, intersection.smooth_normal);
                normal = glm::normalize(tangent_space_to_world * tex_normal);
            }
            BSDF* shader = new LambertianBSDF(normal, albedo);
            return shader;
        }
};

class ReflectionMaterial: public Material {
    public:
        glm::vec3 albedo = glm::vec3(.7f);
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
        ReflectionMaterial(){};
        BSDF* create_shader(const IntersectionData& intersection) final {
            glm::vec3 albedo = this->albedo;
            if (albedo_texture != nullptr) {
                albedo = albedo_texture->sample(intersection.tex_coord);
            }

            glm::vec3 normal = intersection.smooth_normal;
            if (normal_texture != nullptr) {
                glm::vec3 tex_normal = normal_texture->sample(intersection.tex_coord) * 2.0f - 1.0f;
                glm::mat3 tangent_space_to_world = glm::mat3(intersection.tangent, intersection.bitangent, intersection.smooth_normal);
                normal = glm::normalize(tangent_space_to_world * tex_normal);
            }
            BSDF* shader = new ReflectionBSDF(normal, albedo);
            return shader;
        }
};



#endif