#include <iostream>

#include "glm/glm.hpp"  
#include "glm/gtx/string_cast.hpp"
#include "ray.h"
#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "scene.h"
#include "bvh.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"



int main(int argc, char** argv){

    //std::cout << "hello World" << std::endl;

    int width = 512;
    int height = 512;

    Scene scene;
    Camera camera(30, 1);
    BVH bvh;

    Mesh mesh = Mesh::loadObj("test/dragon.obj");
    scene.addMesh(mesh);


    std::cout << "building bvh" << std::endl;
      

    
    for (int i = 0; i < mesh.face_indices.size(); i+=3){
        bvh.triangles.push_back(Triangle(&mesh, i));
    } 
    bvh.build();
   
    
    glm::vec3 *pixels = new glm::vec3[width * height];

    std::cout << bvh.triangles.size() << std::endl;
   
    
    std::cout << "rendering" <<std::endl;
    for (int y = 0; y < height; y++){
        
        for (int x = 0; x < width; x++){

            

            float u = (float)x / (float)width * 2 - 1;
            float v = -((float)y / (float)height * 2 - 1);
            int index = y * width + x;

            Ray camera_ray = camera.generateRay(u,v);

            pixels[index] = glm::vec3(0.f,0.f,0.f);

          
            
            IntersectionData intersection = bvh.nearestIntersection(camera_ray);
            
            if (intersection.t < TMAX){
                pixels[index] = (intersection.triangle.smooth_normal(intersection.bu, intersection.bv) + 1.f)/2.f;
                
                //pixels[index] = glm::vec3(1.f,1.f,1.f);
            } 

         
            
        }
    }

    unsigned char* image_output_buffer = new unsigned char[width * height * 4];

      for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
          
            int index = y * width + x;
            glm::vec3 pixel = pixels[index];
            pixel = glm::clamp(pixel, 0.f, 1.f);
            
            image_output_buffer[index * 4]     = (unsigned char) (pixel.x * 255);
            image_output_buffer[index * 4 + 1] = (unsigned char) (pixel.y * 255);
            image_output_buffer[index * 4 + 2] = (unsigned char) (pixel.z * 255);
            image_output_buffer[index * 4 + 3] = (unsigned char) (255);
        }
    }

    std::cout << "saving" <<std::endl;
    stbi_write_png("test.png", width, height, 4, image_output_buffer, width * 4);

    delete[] pixels;
    delete[] image_output_buffer;

    std::cout << "success" << std::endl;

    return 0;
}
