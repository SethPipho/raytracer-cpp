#include <iostream>
#include <random>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "argparse/argparse.hpp"

#include "glm/glm.hpp"  
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"

#include "geometry/geometry.h"
#include "camera.h"

#include "scene.h"
#include "bvh.h"
#include "render.h"
#include "misc.h"

int main(int argc, char** argv){

    std::cout << "Hello World!" << std::endl;

    argparse::ArgumentParser cli("raytracer-cpp");
    cli.add_argument("-o","--output").default_value(std::string("output.png")).help("Output file");
    cli.add_argument("-s","--scene").help("Scene file to render");
    cli.add_argument("-w","--width").default_value(512).help("Width of output image").scan<'i', int>();
    cli.add_argument("-h","--height").default_value(512).help("Height of output image").scan<'i', int>();
    cli.add_argument("--spp").default_value(64).help("Number of samples per pixel").scan<'i', int>();

    try {
        cli.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << cli;
        std::exit(0);
    }

    int width = cli.get<int>("--width");
    int height = cli.get<int>("--height");
    int spp = cli.get<int>("--spp");
    std::string scene_file = cli.get<std::string>("--scene");
    std::string output_file = cli.get<std::string>("--output");

    std::cout << scene_file << std::endl;
    std::cout << width << "x" << height << " " << spp << "spp" << std::endl;
    std::cout << output_file << std::endl;

    
    std::random_device rd;
    std::minstd_rand gen(rd());
    std::uniform_real_distribution<float> dist(0, 1);

    Scene scene = Scene::load_file(scene_file);
    scene.camera.aspect_ratio = float(width)/float(height);
    scene.build();

    std::cout <<"# Tris: "<< scene.triangles.size() << std::endl;
    
    NeePathTracer renderer;
    glm::vec3 *accumulator = new glm::vec3[width * height];
   
    ProgressBar progress_bar;
    progress_bar.begin();

    std::cout << "rendering" <<std::endl;
    for (int y = 0; y < height; y++){
        for (int x = 0; x < width; x++){

            float u =  (float) x / (float) width  * 2 - 1;
            float v = -((float) y / (float) height * 2 - 1);
            int index = y * width + x;
            accumulator[index] = glm::vec3(0.f,0.f,0.f);

            for (int s = 0; s < spp; s++){
                float aa_x = dist(gen) / (float) width;
                float aa_y = dist(gen) / (float) height;

                Ray camera_ray = scene.camera.generateRay(u + aa_x,v + aa_y); 
                accumulator[index] += glm::clamp(renderer.trace(camera_ray, scene, gen), 0.f, 20.f);
            }
            
        }
        if (y % 32 == 0){
           progress_bar.update(double(y)/ double(height));
           progress_bar.display();
        }        
    }

    progress_bar.update(1.0);
    progress_bar.display();


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
    stbi_write_png(output_file.c_str(), width, height, 4, image_output_buffer, width * 4);

    delete[] accumulator;
    delete[] image_output_buffer;

    std::cout << "success" << std::endl;
    
    

    return 0;
}
