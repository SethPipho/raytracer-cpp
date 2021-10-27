#ifndef UTIL_H_
#define UTIL_H_

#include <vector>

#include "glm/glm.hpp"  
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"


glm::vec3 vector_to_vec3(const std::vector<float>& v){
    return glm::vec3(v.at(0), v.at(1), v.at(2));
}

#endif