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
        bool sample_light = true;
        BSDF(){}
        virtual BSDFSample sample(const glm::vec3& wo, const IntersectionData& intersection) = 0;
        virtual glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) = 0;
        virtual float pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) = 0;
};


class LambertianBSDF : public BSDF {
    public:
        TextureMap* albedo_texture = nullptr;
        TextureMap* normal_texture = nullptr;
         glm::vec3 albedo = glm::vec3(.7f);
        LambertianBSDF(){}
        BSDFSample sample(const glm::vec3& wo, const IntersectionData& intersection) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) final;
};

inline BSDFSample LambertianBSDF::sample(const glm::vec3& wo, const IntersectionData& intersection) {
    BSDFSample sample;
    glm::vec3 direction = sampleSphereUniform(randuf(), randuf());
    if (glm::dot(direction, intersection.smooth_normal) < 0.0f) {
        direction = -direction;
    }
    sample.throughput = eval(wo, direction, intersection);
    sample.direction = direction;
    sample.pdf = pdf(wo, direction, intersection);
    return sample;
}

inline glm::vec3 LambertianBSDF::eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) {
    glm::vec3 albedo;
    if (albedo_texture != nullptr) {
        albedo = albedo_texture->sample(intersection.tex_coord);
        //albedo = glm::vec3(intersection.tex_coord.x, intersection.tex_coord.y, 0.0);
    } else {
        albedo = glm::vec3(.7f);
    }

    glm::vec3 normal;
    if (normal_texture != nullptr) {
        glm::vec3 tex_normal = normal_texture->sample(intersection.tex_coord) * 2.0f - 1.0f;

    
        glm::mat3 tangent_space_to_world = glm::mat3(intersection.tangent, intersection.bitangent, intersection.smooth_normal);

        normal = glm::normalize(tangent_space_to_world * tex_normal);


        //albedo = glm::vec3(intersection.tex_coord.x, intersection.tex_coord.y, 0.0);
    } else {
       normal = intersection.smooth_normal;
    }

    //float v = glm::dot(intersection.smooth_normal, intersection.tangent);
    //albedo = glm::clamp(glm::vec3(v), 0.f, 1.f);

    //albedo = glm::clamp(normal * .5f + .5f, 0.f, 1.f);
    
    //return albedo;
    //albedo = glm::vec3(.7f);

    float v = glm::dot(normal, wi);
    if (v < 0.0f) {
        return glm::vec3(0.0f);
    }
    

    return albedo/PI * v;
}

inline float LambertianBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) {
    return 1.0f / PI;
}


/*
class ReflectionBSDF : public BSDF {
    public:
        ReflectionBSDF(){
            sample_light = false;
        }
        BSDFSample sample(const glm::vec3& wo, const glm::vec3& normal) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi, const glm::vec3& normal) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi, const glm::vec3& normal) final;
};

BSDFSample ReflectionBSDF::sample(const glm::vec3& wo, const glm::vec3& normal) {
    BSDFSample sample;
    glm::vec3 direction = glm::reflect(-wo, normal);
    sample.throughput = glm::vec3(1.0f);
    sample.direction = direction;
    sample.pdf = 1.0f;
    return sample;
}

glm::vec3 ReflectionBSDF::eval(const glm::vec3& wo, const glm::vec3& wi, const glm::vec3& normal) {
    return glm::vec3(0.0f);
}

float ReflectionBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi, const glm::vec3& normal) {
    return 1.0f;
}
*/

#endif