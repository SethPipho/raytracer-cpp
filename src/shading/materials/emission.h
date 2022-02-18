#ifndef EMMISSION_H_
#define EMMISSION_H_

#include "shading/material.h"
#include "shading/texture.h"

class EmissionMaterial: public Material {
    public:
        glm::vec3 emission = glm::vec3(1.0f);
        EmissionMaterial();
        EmissionMaterial(glm::vec3 emission);
        BSDF* create_shader(const IntersectionData& intersection) final;
};


#endif