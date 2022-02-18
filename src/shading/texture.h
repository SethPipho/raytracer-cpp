#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#include <iostream>
#include <string>
#include "glm/glm.hpp"

class TextureMap {
    public:
        int w,h,n;
        glm::vec3* data;
        TextureMap(){};
        ~TextureMap(){
            delete[] data;
        };
        glm::vec3 sample(glm::vec2 uv);
        static TextureMap* load_file(std::string filename);
};



#endif