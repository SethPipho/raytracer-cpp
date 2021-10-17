#ifndef CAMERA_H_
#define CAMERA_H_

#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"

#include "ray.h"

/*
Camera generates ray from given image clip space coordinates
(u,v) which range from -1 to 1.
(-1,1)----------------(1,1)
    ¦                  ¦
    ¦                  ¦
    ¦                  ¦
    ¦                  ¦
    ¦                  ¦
(-1,-1)----------------(-1,1)
*/


class Camera {
    public:
        float fov;
        float h;
        float aspect_ratio;
        glm::mat3 rotation;
        glm::vec3 pos;
        Camera(float fov, float aspect_ratio);
        Ray generateRay(float u, float v);
};

Camera::Camera(float fov, float aspect_ratio){
    this->fov = fov;
    this->aspect_ratio = aspect_ratio;
    this->h = std::tan(fov/180 * 3.1415);
    this->pos = glm::vec3(-1.2f, .5f, 0.f);
    this->rotation = glm::inverse(glm::lookAtRH(this->pos, glm::vec3(0.f, .5f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
}

Ray Camera::generateRay(float u, float v){
    glm::vec3 dir = this->rotation * glm::vec3(u * this-> h * this->aspect_ratio, v * h, -1.f);
  
    //std::cout << glm::to_string(dir) << std::endl;
    return Ray(this->pos, dir);
}


#endif

