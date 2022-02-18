#include "shading/materials/emission.h"

EmissionMaterial::EmissionMaterial() {
    this->emmissive = true;
}



EmissionMaterial::EmissionMaterial(glm::vec3 emission) {
    this->emission = emission;
    this->emmissive = true;
}

BSDF* EmissionMaterial::create_shader(const IntersectionData& intersection) {
    return nullptr;
}

