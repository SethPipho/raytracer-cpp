#ifndef RAY_H_
#define RAY_H_

#include "glm/vec3.hpp"  

class Ray {
    public:
        glm::vec3 origin,direction;
        Ray(): origin(glm::vec3()), direction(glm::vec3()) {}
        Ray(glm::vec3 origin, glm::vec3 direction): origin(origin), direction(direction) {} 
};
#endif