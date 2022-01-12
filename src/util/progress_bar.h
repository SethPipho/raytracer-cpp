#ifndef PBAR_H_
#define PBAR_H_

#include <cstdlib>
#include <cmath>
#include <chrono>
#include <iostream>

double currentTimeMilliseconds(){
    auto now = std::chrono::high_resolution_clock::now().time_since_epoch();
    return (double) std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
}


class ProgressBar {
    public:
        double start_time;
        double last_time;
        double eta; 
        double progress;
       
       
        void begin(){
            start_time = currentTimeMilliseconds();
            last_time = start_time;
            progress = 0.0;
        }
        void update(double _progress){
            double now = currentTimeMilliseconds();
            double elapsed = now - last_time;
            double progress_change = _progress - progress;
            eta = (elapsed/progress_change) * (1 - progress);
            
            last_time = now;
            progress = _progress;
        }
        void display(){
            double elapsed = currentTimeMilliseconds() - start_time;
            elapsed /= 1000;

            double width = 20;
            int num_chars = (int) width * progress;
            
            int elapsed_minutes = (int) elapsed / 60; 
            int elapsed_seconds = (int) elapsed % 60; 

            int eta_minutes = (int) (eta / 1000) / 60;
            int eta_seconds = (int) (eta / 1000) % 60;

            int percent = (int) (progress * 100);

            printf("   [");

            for (int i = 0; i < num_chars; i++){
                printf("/");
            }
            for (int i = num_chars; i < width; i++){
                printf("-");
            }
            
            printf("] %d% | %d:%02d | %d:%02d          \r", percent, elapsed_minutes, elapsed_seconds, eta_minutes, eta_seconds);
            std::cout.flush();

        }


};

#endif