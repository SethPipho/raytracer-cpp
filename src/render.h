#ifndef RENDERER_H_
#define RENDERER_H_

#include <random>

#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "ray.h"
#include "scene.h"
#include "mesh.h"
#include "bvh.h"
#include "geometry.h"

class Renderer {
    public:
        virtual glm::vec3 trace(Ray& ray, Scene& scene, std::minstd_rand gen);

};

class BruteForcePathTracer: public Renderer {
    public:
        //std::minstd_rand gen(std::random_device());
        //std::uniform_real_distribution<float> dist(0.f, 1.f);

        BruteForcePathTracer(){};
        glm::vec3 trace(Ray& ray, Scene& scene, std::minstd_rand gen);
};

glm::vec3 sampleSphereUniform(float r1, float r2){
    float z = r1 * 2 - 1;
    float t = r2 * 2 * 3.1415f;
    float r = std::sqrt(1 - z * z);
    float x = r * std::cos(t);
    float y = r * std::sin(t);
    return glm::vec3(x, y, z);
}

glm::vec3 BruteForcePathTracer::trace(Ray& ray, Scene& scene, std::minstd_rand gen){

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

        glm::vec3 normal = intersection.triangle.smooth_normal(intersection.bu, intersection.bv);
        if (intersection.backface){
            normal *= -1.f;
        }

        glm::vec3 scatter_dir = sampleSphereUniform(dist(gen), dist(gen));
        if (glm::dot(scatter_dir, normal) < 0.f){
            scatter_dir *= -1.f;
        }

        throughput *= intersection.triangle.mesh->color * glm::dot(normal, scatter_dir);

        scatter_ray = Ray(intersection.position, scatter_dir);

        //std::cout << i << " " << glm::to_string(throughput) << std::endl; 
    
    }

    return radiance;
}

#endif