#include <iostream>

#include "glm/vec3.hpp"  
#include "glm/gtx/string_cast.hpp"
#include "ray.h"
#include "camera.h"
#include "geometry.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

int main(int argc, char** argv){

    std::cout << "hello World" << std::endl;

    int width = 512;
    int height = 512;

    Camera camera(45, 1);

    Mesh mesh;
    

    mesh.vertices.push_back(glm::vec3(2,0,0));
    mesh.vertices.push_back(glm::vec3(2,0,1));
    mesh.vertices.push_back(glm::vec3(2,1,0));

    mesh.face_indices.push_back(0);
    mesh.face_indices.push_back(1);
    mesh.face_indices.push_back(2);

    Triangle triangle(&mesh, 0);

    glm::vec3 *pixels = new glm::vec3[width * height];

    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){

            float u = (float)x / (float)width * 2 - 1;
            float v = -((float)y / (float)height * 2 - 1);
            int index = y * width + x;

            Ray camera_ray = camera.generateRay(u,v);

            if (rayTriangleIntersection(camera_ray, triangle)){
                pixels[index] = glm::vec3(1.f,1.f,1.f);
            } else {
                pixels[index] = glm::vec3(0.f,0.f,0.f);
            }

           
            
        }
    }

    unsigned char* image_output_buffer = new unsigned char[width * height * 4];

      for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){
            int index = y * width + x;
            glm::vec3 pixel = pixels[index];
            
            image_output_buffer[index * 4]     = (unsigned char) (pixel.x * 255);
            image_output_buffer[index * 4 + 1] = (unsigned char) (pixel.y * 255);
            image_output_buffer[index * 4 + 2] = (unsigned char) (pixel.z * 255);
            image_output_buffer[index * 4 + 3] = (unsigned char) (255);
        }
    }

    stbi_write_png("test.png", width, height, 4, image_output_buffer, width * 4);

    delete[] pixels;
    delete[] image_output_buffer;

    return 0;
}
