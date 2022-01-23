#ifndef _GTLF_LOADER_H_
#define _GTLF_LOADER_H_

#include <vector>

#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE 
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_JSON 
#include "tinygltf/tiny_gltf.h"

#include "geometry/mesh.h"

std::vector<Mesh> load_gltf(std::string filepath);

//utility classes for reading gltf buffers
class IntArrayAdapterBase {
    public:
        virtual unsigned int get(size_t index) = 0;
};

template<typename T>
class IntArrayAdapter: public IntArrayAdapterBase {
    public:
        const unsigned char* data_address;
        const size_t byte_stride;
        const size_t count;
        IntArrayAdapter(const unsigned char* data_address, const size_t byte_stride,  const size_t count): data_address(data_address), byte_stride(byte_stride), count(count){};
        unsigned int get(size_t index) {
            T value =  *(reinterpret_cast<const T*>(this->data_address + index * this->byte_stride));
            return static_cast<unsigned int>(value);
        };
};

class Vec2ArrayAdapter{
    public:
        const unsigned char* data_address;
        const size_t byte_stride;
        const size_t count;
        Vec2ArrayAdapter(const unsigned char* data_address, const size_t byte_stride,  const size_t count): data_address(data_address), byte_stride(byte_stride), count(count){};
        glm::vec2 get(size_t index) {
            glm::vec2 value =  *(reinterpret_cast<const glm::vec2*>(this->data_address + index * this->byte_stride));
            return value;
        };
};


class Vec3ArrayAdapter{
    public:
        const unsigned char* data_address;
        const size_t byte_stride;
        const size_t count;
        Vec3ArrayAdapter(const unsigned char* data_address, const size_t byte_stride,  const size_t count): data_address(data_address), byte_stride(byte_stride), count(count){};
        glm::vec3 get(size_t index) {
            glm::vec3 value =  *(reinterpret_cast<const glm::vec3*>(this->data_address + index * this->byte_stride));
            return value;
        };
};

#endif