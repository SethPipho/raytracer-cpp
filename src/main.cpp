#include <iostream>

#include <glm/vec3.hpp>  
#include <glm/gtx/string_cast.hpp>

int main(int argc, char** argv){

    std::cout << "hello World" << std::endl;

    glm::vec3 a(2.f, 3.f, 5.f);
    glm::vec3 b(2.f, 3.f, 5.f);

    std::cout<< glm::to_string(a + b) << std::endl;
    return 0;
}
