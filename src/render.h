#ifndef RENDERER_H_
#define RENDERER_H_

#include <random>

#include <glm/glm.hpp>
#include "glm/gtx/string_cast.hpp"

#include "scene.h"
#include "bvh.h"
#include "geometry/geometry.h"

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

        glm::vec3 normal = intersection.triangle.smooth_normal(intersection.barycentric);
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


class NeePathTracer: public Renderer {
    public:
        //std::minstd_rand gen(std::random_device());
        //std::uniform_real_distribution<float> dist(0.f, 1.f);

        NeePathTracer(){};
        glm::vec3 trace(Ray& ray, Scene& scene, std::minstd_rand gen);
};

glm::vec3 NeePathTracer::trace(Ray& ray, Scene& scene, std::minstd_rand gen){

    std::uniform_real_distribution<float> dist(0, 1);

    glm::vec3 radiance = glm::vec3(0.f);
    glm::vec3 throughput = glm::vec3(1.f);
    
    Ray scatter_ray = Ray(ray.origin, ray.direction);

    for (int i = 0; i < 3; i++){
        IntersectionData intersection = scene.bvh.nearestIntersection(scatter_ray);
        if (intersection.t == TMAX){
            break;  
        }

        if (intersection.triangle.mesh->is_light){
            if (i == 0){
                return glm::vec3(15.f) * throughput;
            }
            break;    
        }

        bool backface = false;
        glm::vec3 normal = intersection.triangle.smooth_normal(intersection.barycentric);
        if (glm::dot(ray.direction, normal) >= 0.0f){
            normal *= -1.f;
            backface = true;
        }

        //direct lighting
        Triangle& light = scene.pickLight(dist(gen));
        glm::vec3 light_point = light.sample_point(dist(gen), dist(gen));
        glm::vec3 light_normal = light.flat_normal();
        glm::vec3 to_light = light_point - intersection.position;
        float light_dist = glm::length(to_light);
        to_light /= light_dist;


        float light_pos_pdf = 1.0f/ light.area();
        float light_pick_pdf = 1.0f/scene.lights.size();

        Ray shadow_ray = Ray(intersection.position, to_light);

        bool occluded = (scene.bvh.isOccluded(shadow_ray, light_dist - .0001f));

        if (!occluded){
            
            
            float solid_angle = glm::dot(to_light, light_normal) / (light_dist * light_dist);
            glm::vec3 bsdf_eval = intersection.triangle.mesh->color/3.14f * glm::dot(normal, to_light);


            if (glm::dot(to_light, normal) < 0.0f){
                bsdf_eval *= -1.f;
            }
           
            if (solid_angle < 0.f){
                solid_angle *= -1.f;
            }
         
    
            float light_pdf = light_pos_pdf * light_pick_pdf;
            glm::vec3 direct_lighting = glm::vec3(35.f) * bsdf_eval * solid_angle / light_pdf;
            radiance += throughput * direct_lighting;
        } 

        //indirect lighting
        glm::vec3 scatter_dir = sampleSphereUniform(dist(gen), dist(gen));
        if (glm::dot(scatter_dir, normal) < 0.f){
            scatter_dir *= -1.f;
        }

        throughput *= intersection.triangle.mesh->color * glm::dot(normal, scatter_dir);

        scatter_ray = Ray(intersection.position, scatter_dir);

        //std::cout << i << " " << glm::to_string(throughput) << std::endl; 
    
    }

    assert(radiance.x >= 0.f);
    assert(radiance.y >= 0.f);
    assert(radiance.z >= 0.f);
    return radiance;
}

#endif