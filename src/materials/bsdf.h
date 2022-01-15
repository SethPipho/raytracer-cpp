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
        glm::vec3 albedo;
        bool is_light = false;
        bool sample_light = true;
        BSDF(){}
        virtual BSDFSample sample(const glm::vec3& wo, const IntersectionData& intersection) = 0;
        virtual glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) = 0;
        virtual float pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) = 0;
};


class LambertianBSDF : public BSDF {
    public:
        bool use_texture = false;
        TextureMap* albedo_texture;
        LambertianBSDF(){}
        BSDFSample sample(const glm::vec3& wo, const IntersectionData& intersection) final;
        glm::vec3 eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) final;
        float pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) final;
};

BSDFSample LambertianBSDF::sample(const glm::vec3& wo, const IntersectionData& intersection) {
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

glm::vec3 LambertianBSDF::eval(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) {
    glm::vec3 albedo;
    if (use_texture) {
        albedo = albedo_texture->sample(intersection.tex_coord);
        //albedo = glm::vec3(intersection.tex_coord.x, intersection.tex_coord.y, 0.0);
    } else {
        albedo = this->albedo;
    }
    
    return albedo / PI * glm::dot( intersection.smooth_normal, wi);
}

float LambertianBSDF::pdf(const glm::vec3& wo, const glm::vec3& wi, const IntersectionData& intersection) {
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