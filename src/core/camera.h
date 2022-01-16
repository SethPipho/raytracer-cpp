#ifndef CAMERA_H_
#define CAMERA_H_

#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"
#include "json/json.hpp"
using json = nlohmann::json;


#include "geometry/ray.h"
#include "util/math.h"

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
        glm::mat4 transform;
        glm::vec3 position;
        Camera(){}
        Camera(float fov, float aspect_ratio, glm::mat4 transform);
        Ray generateRay(float u, float v);
        static Camera from_json(json config);
};

inline Camera::Camera(float fov, float aspect_ratio, glm::mat4 transform){
    this->fov = fov;
    this->aspect_ratio = aspect_ratio;
    this->h = std::tan(fov/180 * 3.1415);
    this->transform = transform; 
    this->position = this->transform * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

inline Ray Camera::generateRay(float u, float v){
    glm::vec3 dir = this->transform * glm::vec4(u * this->h * this->aspect_ratio, v * h, -1.f, 0);
    dir = glm::normalize(dir);
    return Ray(this->position, dir);
}

inline Camera Camera::from_json(json config){
    float fov = config["fov"];
    glm::vec3 pos = vector_to_vec3(config["position"]);
    glm::vec3 lookat = vector_to_vec3(config["lookat"]);
    glm::mat4 transform = glm::inverse(glm::lookAtRH(pos, lookat, glm::vec3(0.f, 1.f, 0.f)));
    return Camera(fov, 1.0f, transform);
}

#endif

