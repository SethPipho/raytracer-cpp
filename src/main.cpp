#include <iostream>
#include <random>

#include "glm/glm.hpp"  
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"


#include "ray.h"
#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "scene.h"
#include "bvh.h"
#include "render.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"




int main(int argc, char** argv){

    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_real_distribution<float> dist(0, 1);

    int width = 640 / 2;
    int height = 360 / 2;
    int spp = 16;
    glm::vec3 *accumulator = new glm::vec3[width * height];
    
    BruteForcePathTracer renderer;

    Scene scene = Scene::load_file("test/test.json");
    scene.camera.aspect_ratio = float(width)/float(height);
    scene.build();

    scene.meshes[0].is_light = true;

    scene.meshes[6].color = glm::vec3(1.0f, 0.25, 0.0);
    scene.meshes[7].color = glm::vec3(0.f, .25f, 1.0f);

    
    std::cout <<"# Tris: "<< scene.triangles.size() << std::endl;
   
   
    
    std::cout << "rendering" <<std::endl;
    for (int y = 0; y < height; y++){
        std:: cout << y << std::endl;
        for (int x = 0; x < width; x++){

            float u =  (float) x / (float) width  * 2 - 1;
            float v = -((float) y / (float) height * 2 - 1);
            int index = y * width + x;
            accumulator[index] = glm::vec3(0.f,0.f,0.f);

            for (int s = 0; s < spp; s++){
                float aa_x = dist(gen) / (float) width;
                float aa_y = dist(gen) / (float) height;

                Ray camera_ray = scene.camera.generateRay(u + aa_x,v + aa_y); 
                accumulator[index] += renderer.trace(camera_ray, scene, gen);
            }
            
        }
    }

    unsigned char* image_output_buffer = new unsigned char[width * height * 4];

      for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
          
            int index = y * width + x;
            glm::vec3 pixel = accumulator[index] / (float) spp;
            pixel = pixel / (pixel + glm::vec3(1.f));
            pixel = glm::clamp(pixel, 0.f, 1.f);
            pixel = glm::pow(pixel, glm::vec3(1/2.2f));
            
            image_output_buffer[index * 4]     = (unsigned char) (pixel.x * 254);
            image_output_buffer[index * 4 + 1] = (unsigned char) (pixel.y * 254);
            image_output_buffer[index * 4 + 2] = (unsigned char) (pixel.z * 254);
            image_output_buffer[index * 4 + 3] = (unsigned char) (255);
        }
    }

    std::cout << "saving" <<std::endl;
    stbi_write_png("test.png", width, height, 4, image_output_buffer, width * 4);

    delete[] accumulator;
    delete[] image_output_buffer;

    std::cout << "success" << std::endl;

    return 0;
}
