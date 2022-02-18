#include "integrator/integrator.h"


glm::vec3 NeePathTracer::trace(Ray& primary_ray, Scene& scene){

    glm::vec3 radiance = glm::vec3(0.f);
    glm::vec3 throughput = glm::vec3(1.f);
    
    Ray scatter_ray = Ray(primary_ray.origin, primary_ray.direction);
    bool specular_bounce = false;

    for (int i = 0; i < 5; i++){
        
        IntersectionData intersection = scene.bvh.nearestIntersection(scatter_ray);
     
        
        if (!intersection.hit){
            radiance += throughput * glm::vec3(0.0f);
            break;  
        }

        Material* material = intersection.triangle.mesh->material;


        if (intersection.triangle.mesh->is_light){
            if (i == 0){
                return  static_cast<EmissionMaterial*>(material)->emission * throughput;
            } else if (specular_bounce){
                radiance += throughput * static_cast<EmissionMaterial*>(material)->emission;
            } 
            break;
        }

        bool backface = false;
        
        glm::vec3 normal = intersection.triangle.normal(intersection.barycentric);
     
        if (glm::dot(scatter_ray.direction, normal) >= 0.0f){
            normal *= -1.f;
            backface = true;
        }

        intersection.normal = normal;
        intersection.tex_coord = intersection.triangle.tex_coords(intersection.barycentric);
        intersection.tangent = intersection.triangle.tangent(intersection.barycentric);
        intersection.bitangent = intersection.triangle.bitangent(intersection.barycentric);
     
        BSDF* bsdf = material->create_shader(intersection);

         //direct lighting    
        if (bsdf->sample_light){

            specular_bounce = false;
            
            LightSample light_sample = scene.sampleLight(intersection);
         
        
            Ray shadow_ray = Ray(intersection.position, light_sample.direction);
            bool occluded = scene.bvh.isOccluded(shadow_ray, light_sample.distance - .0001f);

            if (!occluded){

                glm::vec3 bsdf_eval = bsdf->eval(-shadow_ray.direction, light_sample.direction);
                float solid_angle = glm::dot(light_sample.direction, light_sample.normal) / (light_sample.distance * light_sample.distance);
               

                EmissionMaterial* emissive_material = static_cast<EmissionMaterial*>(light_sample.light.mesh->material);
            
                glm::vec3 direct_lighting = emissive_material->emission * bsdf_eval * solid_angle / light_sample.pdf;
                direct_lighting = glm::max(direct_lighting, 0.f);
                radiance += throughput * direct_lighting;
            } 

        } else {
            specular_bounce = true;
        }

        //indirect lighting
        BSDFSample sample = bsdf->sample(-scatter_ray.direction);
        throughput *= sample.throughput / sample.pdf;
        scatter_ray = Ray(intersection.position, sample.direction);
        delete bsdf;
    }

    assert(radiance.x >= 0.f);
    assert(radiance.y >= 0.f);
    assert(radiance.z >= 0.f);
    return radiance;
}
