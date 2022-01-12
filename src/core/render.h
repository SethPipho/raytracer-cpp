#ifndef RENDER_H_
#define RENDER_H_

#include <string>
#include <vector>
#include <thread>

#include "core/scene.h"
#include "integrator/integrator.h"
#include "util/thread_safe_queue.h"
#include "util/math.h"


struct RenderConfig {
    int width;
    int height;
    int spp;
    int max_bounces;
    int num_threads;
    int tile_size;
    std::string output_file;
    std::string scene_file;
};

struct RenderTile {
    int w, h, x, y;
};

void render_tiled_worker(Integrator& integrator, Scene& scene, RenderConfig config, ThreadSafeQueue<RenderTile>& tile_queue, glm::vec3* accumulator){
    RenderTile tile;
    
    while(tile_queue.pop(tile)){
        
         for (int y = tile.y; y < tile.y + tile.h; y++){
            for (int x = tile.x; x < tile.x + tile.w; x++){
                int index = y * config.width + x;
                accumulator[index] = glm::vec3(0.f,0.f,0.f);

                float u =  (float) x / (float) config.width  * 2 - 1;
                float v = -((float) y / (float) config.height * 2 - 1);

                for (int s = 0; s < config.spp; s++){

                    float aa_x = randuf() / (float) config.width;
                    float aa_y = randuf() / (float) config.height;

                    Ray camera_ray = scene.camera.generateRay(u + aa_x,v + aa_y); 
                    accumulator[index] += glm::clamp(integrator.trace(camera_ray, scene), 0.f, 20.f);
                }
            }
        }        
    }
}


void render_tiled(Integrator& integrator, Scene& scene, RenderConfig config, glm::vec3* accumulator){
    
    std::vector<std::thread> threads;
    ThreadSafeQueue<RenderTile> tile_queue;

    for (int i = 0; i < config.num_threads; i++) {
        threads.push_back(std::thread(render_tiled_worker, std::ref(integrator), std::ref(scene), config, std::ref(tile_queue), accumulator));
    }

    //create tiles
    for (int y = 0; y < config.height; y += config.tile_size){
        for (int x = 0; x < config.width; x += config.tile_size){
            RenderTile tile = {config.tile_size, config.tile_size, x, y};
            if (y + config.tile_size > config.height) { tile.h = config.height - y; }
            if (x + config.tile_size > config.width)  { tile.w = config.width - x;  }
            tile_queue.push(tile);
        }
    }

    tile_queue.close();
    for (auto &t : threads) {
        t.join();
    }
}

#endif