#ifndef CAMERA_H_
#define CAMERA_H_

#include <cmath>

#include <glm/glm.hpp>

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
        Camera(float fov, float aspect_ratio);
        Ray generateRay(float u, float v);
};

Camera::Camera(float fov, float aspect_ratio){
    this->fov = fov;
    this->aspect_ratio = aspect_ratio;
    this->h = std::tan(fov/180 * 3.1415);
}

Ray Camera::generateRay(float u, float v){
    glm::vec3 direction = glm::normalize(glm::vec3(1, u * this-> h * this->aspect_ratio, v * h));
    glm::vec3 origin = glm::vec3(0,0,0);

    return Ray(origin, direction);
}


#endif

