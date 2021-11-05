#ifndef BBOX_H_
#define BBOX_H_

#include <iostream>
#include <limits>

#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"

class BBox {
    public:
        glm::vec3 min, max;
        BBox();
        BBox(glm::vec3 min, glm::vec3 max);
        BBox(const BBox& other);
        glm::vec3 centroid();
        float surface_area();
        static BBox unionBBox(const BBox& a, const BBox& b);
};

BBox::BBox(){
    this->min = glm::vec3(std::numeric_limits<float>::infinity());
    this->max = glm::vec3(-std::numeric_limits<float>::infinity());
}

BBox::BBox(glm::vec3 min, glm::vec3 max){
    this->min = min;
    this->max = max;
}

BBox::BBox(const BBox& other){
    this->min = other.min;
    this->max = other.max;
}

glm::vec3 BBox::centroid(){
    return this->min + ((this->max - this->min)/2.0f);
};

BBox BBox::unionBBox(const BBox& a, const BBox& b){
    glm::vec3 minb(std::min(a.min.x, b.min.x), std::min(a.min.y, b.min.y), std::min(a.min.z, b.min.z));
    glm::vec3 maxb(std::max(a.max.x, b.max.x), std::max(a.max.y, b.max.y), std::max(a.max.z, b.max.z));
    return BBox(minb, maxb);
};

float BBox::surface_area(){
    glm::vec3 l = this->max - this->min;
    return (2 * l[0] * l[1]) + (2 * l[0] * l[2]) + (2 * l[1] * l[2]);
};

std::ostream& operator << ( std::ostream& outs, const BBox& box ){
  return outs << "BBox(" << glm::to_string(box.min) << "," << glm::to_string(box.max) << ")";
}

#endif