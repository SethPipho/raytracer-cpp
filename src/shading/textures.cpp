

#include "stb/stb_image.h"
#include "shading/texture.h"

glm::vec3 TextureMap::sample(glm::vec2 uv){
    int x = uv.x * w;
    int y = (1.0f - uv.y) * h;
    int idx = y * w + x;
    return data[idx];
}


TextureMap* TextureMap::load_file(std::string filename) {
    TextureMap* map = new TextureMap;
    int w,h,n;
    unsigned char* data = stbi_load(filename.c_str(), &w, &h, &n, 0);
    if(data == NULL) {
        std::cerr << "Failed to load texture file: " << filename << std::endl;
        std::exit(1);
    }
    map->w = w;
    map->h = h;
    map->n = n;
    map->data = new glm::vec3[w*h];
    for(int i=0; i<w*h; i++) {
        glm::vec3 color =  glm::vec3(data[i*n+0]/254.0f, data[i*n+1]/254.0f, data[i*n+2]/254.0f);
        color = glm::pow(color, glm::vec3(2.2f)); // linear space
        map->data[i] = color;
    }
    stbi_image_free(data);
    return map;
}
