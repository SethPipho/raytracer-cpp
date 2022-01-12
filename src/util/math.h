#ifndef UTIL_MATH_H_
#define UTIL_MATH_H_

#include <vector>
#include <random>

#include "glm/glm.hpp"  
#include <glm/gtx/transform.hpp> 
#include "glm/gtx/string_cast.hpp"

#define PI 3.1415926535897932384626433832795f


inline float randuf(){
    static thread_local std::random_device rd;
    static std::minstd_rand gen(2.0f);
    static std::uniform_real_distribution<float> dist(0, 1);
    return dist(gen);
}


inline glm::vec3 vector_to_vec3(const std::vector<float>& v){
    return glm::vec3(v.at(0), v.at(1), v.at(2));
}


inline glm::vec3 sampleSphereUniform(float r1, float r2){
    float z = r1 * 2 - 1;
    float t = r2 * 2 * 3.1415f;
    float r = std::sqrt(1 - z * z);
    float x = r * std::cos(t);
    float y = r * std::sin(t);
    return glm::vec3(x, y, z);
}

#endif