#ifndef BVH_H_
#define BVH_H_

#include <vector>
#include <limits>
#include <algorithm>

#include "ray.h"
#include "geometry.h"

struct BVHNode {
    BBox bbox;
    int offset;
    int n;
    BVHNode* left;
    BVHNode* right;
};

struct BVHBin {
    int n = 0;
    BBox bbox;
};

class BVH {
    public:
        std::vector<Triangle> triangles;
        BVHNode* root;
        int n_bins = 8;
        std::vector<BVHBin> bins[3];

        BVH(){
            BVHBin bin;
            for (int i = 0; i < 3; i++){
                this->bins[i] = std::vector<BVHBin>(this->n_bins, bin);
            }
        }
        void build();
        void buildRecursive(BVHNode* parent);
        IntersectionData nearestIntersection(Ray& ray);
        void nearestIntersectionRecursive(Ray& ray, IntersectionData* nearest, BVHNode* node);
};

IntersectionData BVH::nearestIntersection(Ray& ray){
    IntersectionData intersection;

    this->nearestIntersectionRecursive(ray, &intersection, this->root);
    
    return intersection;
};

void BVH::nearestIntersectionRecursive(Ray& ray, IntersectionData* nearest, BVHNode* node){
    float t;
    if (!(rayBBoxIntersection(ray, node->bbox, &t))) {
        return;
    }

    //check if leaf
    if (node->left == nullptr){
        IntersectionData intersection;
        for (int i = node->offset; i < node->offset + node->n; i++){
            Triangle& tri = this->triangles[i];
            intersection = rayTriangleIntersection(ray, tri);
            if (intersection.t < nearest->t){
                *nearest = intersection;
            }
            
        }
        return;
    }

    this->nearestIntersectionRecursive(ray, nearest, node->left);
    this->nearestIntersectionRecursive(ray, nearest, node->right);
};


void BVH::build(){
    this->root = new BVHNode();
    this->root->bbox = this->triangles[0].bbox();
    this->root->offset = 0;
    this->root->n = this->triangles.size();
    for (Triangle& t: this->triangles){
        root->bbox = BBox::unionBBox(this->root->bbox, t.bbox());
    }
    this->buildRecursive(this->root);
};

void BVH::buildRecursive(BVHNode* parent){

    if (parent->n < 4){
        return;
    }

    //reset bins
    for (int j = 0; j < 3; j++){
        for (int i = 0; i < this->n_bins; i++){
             this->bins[j][i].n = 0;
        }
    }

    glm::vec3 bbox_side_lengths = parent->bbox.max - parent->bbox.min;

    //bin triangles by centroid
    for (int idx = parent->offset; idx < parent->offset + parent->n; idx++){
        Triangle t = this->triangles[idx];
        glm::vec3 centroid = t.centroid();
        for (int axis = 0; axis < 3; axis++){
            float i = (centroid[axis] - parent->bbox.min[axis])/bbox_side_lengths[axis];
            int bin_num = (int)((float)this->n_bins * i);
            BVHBin& bin = this->bins[axis][bin_num];
            if (bin.n == 0){
                bin.bbox = t.bbox();
            }
            bin.n += 1;
            bin.bbox = BBox::unionBBox(t.bbox(), bin.bbox);
        }
    }

    float min_cost = std::numeric_limits<float>::infinity();
    int min_axis = 0;
    int min_split = 0;
    BBox min_left_bbox;
    BBox min_right_bbox;

    //compute costs for each split
    for (int axis = 0; axis < 3; axis++){
        for (int split = 1; split < this->n_bins-1; split++){

            BBox left_bbox;
            BBox right_bbox;
            int left_n = 0;
            int right_n = 0;
          
            left_bbox = this->bins[axis][0].bbox;
            right_bbox = this->bins[axis][this->n_bins - 1].bbox;

            for(int i = 0; i < split; i++){
                left_bbox = BBox::unionBBox(left_bbox, this->bins[axis][i].bbox);
                left_n += this->bins[axis][i].n;
            }
            for(int i = split; i < this->n_bins; i++){
                right_bbox = BBox::unionBBox(right_bbox, this->bins[axis][i].bbox);
                right_n += this->bins[axis][i].n;
            }

            float p_right = right_bbox.surface_area()/parent->bbox.surface_area();
            float p_left = left_bbox.surface_area()/parent->bbox.surface_area();
            float cost = p_left * left_n + p_right * right_n;

            if (cost < min_cost && left_n > 0 && right_n > 0){
                min_cost = cost;
                min_axis = axis;
                min_split = split;
                min_left_bbox = left_bbox;
                min_right_bbox = right_bbox;
            }
        }
    }

    if (parent->n <= min_cost){
        return;
    }

    float bin_size = bbox_side_lengths[min_axis]/this->n_bins;
    float partition_cord = parent->bbox.min[min_axis] + bin_size * min_split;

    auto it = std::partition(this->triangles.begin() + parent->offset, 
                   this->triangles.begin() + parent->offset + parent->n,
                   [min_axis, partition_cord](Triangle &t){return t.centroid()[min_axis] < partition_cord;});

    int partition_index = (it - this->triangles.begin());

    /*
    for (int i = parent->offset; i < parent->offset + parent->n; i++){
     // std::cout << this->triangles[i].centroid()[min_axis] << std::endl;
    }
    std::cout << min_axis << std::endl;
    std::cout << partition_cord << std::endl;
    std::cout << partition_index << std::endl;
    std::cout << min_left_bbox << std::endl << min_right_bbox << std::endl;
    */

    BVHNode* left = new BVHNode();
    BVHNode* right = new BVHNode();

    left->offset = parent->offset;
    left->n = partition_index - parent->offset;
    left->bbox = min_left_bbox;

    right->offset = partition_index;
    right->n = parent->n - left->n;
    right->bbox = min_right_bbox;

  

   // if (left->n < 1 || right->n < 1){
    //    return;
   // }

    parent->right = right;
    parent->left = left;

    this->buildRecursive(left);
    this->buildRecursive(right);
    
};



#endif