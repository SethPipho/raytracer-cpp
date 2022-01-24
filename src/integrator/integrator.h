#ifndef INTEGRATOR_H_
#define INTEGRATOR_H_

#include <random>

#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "core/scene.h"
#include "geometry/geometry.h"
#include "util/math.h"
#include "materials/material.h"

class Integrator {
    public:
        virtual glm::vec3 trace(Ray& ray, Scene& scene) = 0;
};

/*
class BruteForcePathTracer: public Renderer {
    public:
       
        BruteForcePathTracer(){};
        glm::vec3 trace(Ray& ray, Scene& scene);
};


glm::vec3 BruteForcePathTracer::trace(Ray& ray, Scene& scene){

    std::uniform_real_distribution<float> dist(0, 1);

    glm::vec3 radiance = glm::vec3(0.f);
    glm::vec3 throughput = glm::vec3(1.f);
    
    Ray scatter_ray = Ray(ray.origin, ray.direction);

    for (int i = 0; i < 5; i++){
        IntersectionData intersection = scene.bvh.nearestIntersection(scatter_ray);
        if (intersection.t == TMAX){
            return glm::vec3(0.f);    
        }
        if (intersection.triangle.mesh->is_light){
            return glm::vec3(15.f) * throughput;
        }

        glm::vec3 normal = intersection.triangle.smooth_normal(intersection.barycentric);
        if (intersection.backface){
            normal *= -1.f;
        }

        glm::vec3 scatter_dir = sampleSphereUniform(randuf(), randuf());
        if (glm::dot(scatter_dir, normal) < 0.f){
            scatter_dir *= -1.f;
        }

        throughput *= intersection.triangle.mesh->color * glm::dot(normal, scatter_dir);

        scatter_ray = Ray(intersection.position, scatter_dir);

        //std::cout << i << " " << glm::to_string(throughput) << std::endl; 
    
    }

    return radiance;
}

*/

class NeePathTracer: public Integrator {
    public:

        NeePathTracer(){};
        glm::vec3 trace(Ray& ray, Scene& scene);
};

#endif