#ifndef SCENE_H_
#define SCENE_H_

#include <vector>
#include "glm/glm.hpp"
#include "geometry.h"

class Scene {
    public:
        std::vector<Triangle> triangles;

        Scene(){};
        void addMesh(Mesh& mesh);
        IntersectionData nearestIntersection(Ray& r);
};

void Scene::addMesh(Mesh& mesh){
     for (int i = 0; i < mesh.face_indices.size(); i+=3){
        Triangle t(&mesh, i);
        this->triangles.push_back(t);
    } 
};

IntersectionData Scene::nearestIntersection(Ray& ray){
    IntersectionData intersection;
    IntersectionData nearest_intersection;

    for (Triangle t: this->triangles){
        if (rayTriangleIntersection(ray, t, 0, 100000, &intersection)){
            if (intersection.t < nearest_intersection.t){
                 nearest_intersection = intersection;
            }
        }
    }
    return nearest_intersection;
}

#endif