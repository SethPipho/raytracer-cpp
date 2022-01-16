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


#endif