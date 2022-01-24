#include "integrator/integrator.h"


glm::vec3 NeePathTracer::trace(Ray& primary_ray, Scene& scene){

    glm::vec3 radiance = glm::vec3(0.f);
    glm::vec3 throughput = glm::vec3(1.f);
    
    Ray scatter_ray = Ray(primary_ray.origin, primary_ray.direction);

    bool specular_bounce = false;

    for (int i = 0; i < 3; i++){
        IntersectionData intersection = scene.bvh.nearestIntersection(scatter_ray);
        if (!intersection.hit){
            radiance += throughput * glm::vec3(0.0f);
            break;  
        }

        if (intersection.triangle.mesh->is_light){
            if (i == 0){
                return glm::vec3(20.f) * throughput;
            } else if (specular_bounce){
                radiance += throughput * glm::vec3(20.f);
            } 
            break;
        }

        bool backface = false;
        
        glm::vec3 normal = intersection.triangle.smooth_normal(intersection.barycentric);
     
        if (glm::dot(scatter_ray.direction, normal) >= 0.0f){
            normal *= -1.f;
            backface = true;
        }

       
        
        intersection.smooth_normal = normal;
        intersection.tex_coord = intersection.triangle.tex_coords(intersection.barycentric);
        intersection.tangent = intersection.triangle.tangent(intersection.barycentric);
        intersection.bitangent = intersection.triangle.bitangent(intersection.barycentric);

        //direct lighting
        Triangle& light = scene.pickLight(randuf());
        float u = randuf();
        float v = randuf();
        glm::vec3 light_point = light.sample_point(u,v);
        glm::vec3 light_normal = light.smooth_normal(glm::vec2(u,v));
        glm::vec3 to_light = light_point - intersection.position;
        float light_dist = glm::length(to_light);
        to_light /= light_dist;


        float light_pos_pdf = 1.0f/ light.area();
        float light_pick_pdf = 1.0f/scene.lights.size();

        Ray shadow_ray = Ray(intersection.position, to_light);

        Material* material = intersection.triangle.mesh->material;
        BSDF* bsdf = material->create_shader(intersection);

        
        if (bsdf->sample_light){

              
            specular_bounce = false;
            bool occluded = (scene.bvh.isOccluded(shadow_ray, light_dist - .0001f));

        


            if (!occluded){

                float solid_angle = glm::dot(to_light, light_normal) / (light_dist * light_dist);
                if (solid_angle < 0.f) solid_angle *= -1.f;
              

                
                glm::vec3 bsdf_eval = bsdf->eval(-shadow_ray.direction, to_light);
                bsdf_eval = glm::clamp(bsdf_eval, 0.f, 1.f);
            

                float light_pdf = light_pos_pdf * light_pick_pdf;
                glm::vec3 direct_lighting = glm::vec3(20.f) * bsdf_eval * solid_angle / light_pdf;
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

    //assert(radiance.x >= 0.f);
   // assert(radiance.y >= 0.f);
    //assert(radiance.z >= 0.f);
    return radiance;
}
