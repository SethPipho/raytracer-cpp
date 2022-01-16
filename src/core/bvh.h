#ifndef BVH_H_
#define BVH_H_

#include <vector>
#include <limits>
#include <algorithm>
#include <cmath>

#include "geometry/geometry.h"

struct BVHNode {
    BBox bbox;
    int offset;
    int n;
    BVHNode* left;
    BVHNode* right;
};


class BVH {

    struct Bin {
        int n = 0;
        BBox bbox;
    };

    struct Split {
        BBox left_bbox;
        BBox right_bbox;
        int left_n = 0;
        int right_n = 0;
    };

    public:
        std::vector<Triangle>* triangles = nullptr;
        BVHNode* root = nullptr;
        int n_bins = 128;
        std::vector<Bin> bins[3];
        std::vector<Split> splits[3];

        BVH(){}
        BVH(std::vector<Triangle>* triangles){
            this->triangles = triangles;
            for (int i = 0; i < 3; i++){
                this->bins[i] = std::vector<Bin>(this->n_bins, Bin());
                this->splits[i] = std::vector<Split>(this->n_bins - 1, Split());
            }
        }

        ~BVH(){
            this->free_nodes(this->root);
        }

        void free_nodes(BVHNode* parent){
            if (parent == nullptr){
                return;
            }
            this->free_nodes(parent->left);
            this->free_nodes(parent->right);
            delete parent;
        }

        void build();
        void buildRecursive(BVHNode* parent);
        IntersectionData nearestIntersection(Ray& ray);
        void nearestIntersectionRecursive(Ray& ray, IntersectionData* nearest, BVHNode* node, int depth);
        bool isOccluded(Ray& ray, float dist);
        bool isOccludedRecursive(Ray& ray, float dist, BVHNode* node);
};




#endif