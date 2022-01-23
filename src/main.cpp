#include <iostream>
#include <random>
#include <string>
#include <vector>
#include <thread>
#include <chrono>

//#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"
#include "argparse/argparse.hpp"

#include "core/camera.h"
#include "core/scene.h"
#include "core/render.h"
#include "integrator/integrator.h"
#include "util/progress_bar.h"


int main(int argc, char** argv){

    argparse::ArgumentParser cli("raytracer-cpp");
    cli.add_argument("-o","--output").default_value(std::string("output.png")).help("Output file");
    cli.add_argument("-s","--scene").help("Scene file to render");
    cli.add_argument("-w","--width").default_value(512).help("Width of output image").scan<'i', int>();
    cli.add_argument("-h","--height").default_value(512).help("Height of output image").scan<'i', int>();
    cli.add_argument("--spp").default_value(64).help("Number of samples per pixel").scan<'i', int>();
    cli.add_argument("--tile-size").default_value(16).help("Tile Size").scan<'i', int>();

    try {
        cli.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << cli;
        std::exit(0);
    }

    RenderConfig config;
    config.width = cli.get<int>("--width");
    config.height = cli.get<int>("--height");
    config.spp = cli.get<int>("--spp");
    config.scene_file = cli.get<std::string>("--scene");
    config.output_file = cli.get<std::string>("--output");
    config.tile_size = cli.get<int>("--tile-size");
    config.num_threads = std::thread::hardware_concurrency();

    std::cout << config.scene_file << std::endl;
    std::cout << config.output_file << std::endl;
    std::cout << config.width << "x" << config.height << " " << config.spp << "spp" << std::endl;
    std::cout << config.tile_size << "x" <<  config.tile_size << " tiles" << std::endl;
    std::cout << config.num_threads << " threads available" << std::endl;
  
    //Scene scene = Scene::load_gltf(config.scene_file);
   
    
    Scene scene = Scene::load_file(config.scene_file);
    

    scene.camera.aspect_ratio = float(config.width)/float(config.height);
    scene.build();

    std::cout <<"# Tris: "<< scene.triangles.size() << std::endl;
    
    NeePathTracer integrator;
    glm::vec3 *accumulator = new glm::vec3[config.width * config.height];
   
    ProgressBar progress_bar;
    progress_bar.begin();

    std::cout << "rendering" <<std::endl;

    render_tiled(integrator, scene, config, accumulator);

 
    progress_bar.update(1.0);
    progress_bar.display();

    
    unsigned char* image_output_buffer = new unsigned char[config.width * config.height * 4];

      for (int y = 0; y < config.height; y++){
        for (int x = 0; x < config.width; x++){
          
            int index = y * config.width + x;
            glm::vec3 pixel = accumulator[index] / (float) config.spp;
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
    stbi_write_png(config.output_file.c_str(), config.width, config.height, 4, image_output_buffer, config.width * 4);

    delete[] accumulator;
    delete[] image_output_buffer;

    std::cout << "success" << std::endl;
    
    

    return 0;
    
}
