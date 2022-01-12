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

bool BVH::isOccluded(Ray& ray, float dist){
    return this->isOccludedRecursive(ray, dist, this->root);
}

bool BVH::isOccludedRecursive(Ray& ray, float dist, BVHNode* node){
    float t;
    bool hit = rayBBoxIntersection(ray, node->bbox, &t);
    if (!hit || t > dist){
        return false;
    }
    if (node->right == nullptr){
        for (int i = node->offset; i < node->offset + node->n; i++){
            Triangle& tri = (*this->triangles).at(i);
            IntersectionData intersection = rayTriangleIntersection(ray, tri);
            if (intersection.hit && intersection.t < dist){
                return true;
            }
        }
        return false;
    }
    return this->isOccludedRecursive(ray, dist, node->left) || this->isOccludedRecursive(ray, dist, node->right);
}

IntersectionData BVH::nearestIntersection(Ray& ray){
    IntersectionData intersection;

    this->nearestIntersectionRecursive(ray, &intersection, this->root, 0);
    
    return intersection;
};

void BVH::nearestIntersectionRecursive(Ray& ray, IntersectionData* nearest, BVHNode* node, int depth){
    float t;
    if (!(rayBBoxIntersection(ray, node->bbox, &t))) {
        return;
    }
    if (t > nearest->t){
        return;
    }
    //check if leaf
    if (node->left == nullptr){
        IntersectionData intersection;
        for (int i = node->offset; i < node->offset + node->n; i++){
            Triangle& tri = (*this->triangles)[i];
            intersection = rayTriangleIntersection(ray, tri);
            if (intersection.t < nearest->t){
                *nearest = intersection;
                nearest->depth = depth;
            }
            
        }
        return;
    }
    this->nearestIntersectionRecursive(ray, nearest, node->left, depth + 1);
    this->nearestIntersectionRecursive(ray, nearest, node->right, depth + 1);
};


void BVH::build(){
    this->root = new BVHNode();
    this->root->bbox = (*this->triangles)[0].bbox();
    this->root->offset = 0;
    this->root->n = this->triangles->size();
    for (Triangle& t: (*this->triangles)){
        root->bbox = BBox::unionBBox(this->root->bbox, t.bbox());
    }
    this->buildRecursive(this->root);
};

void BVH::buildRecursive(BVHNode* parent){

    if (parent->n <= 4){
        return;
    }

   for (int i = 0; i < 3; i++){
        this->bins[i] = std::vector<Bin>(this->n_bins, Bin());
        this->splits[i] = std::vector<Split>(this->n_bins - 1, Split());
    }

    glm::vec3 bbox_side_lengths = parent->bbox.max - parent->bbox.min;

    //bin triangles by centroid
    for (int idx = parent->offset; idx < parent->offset + parent->n; idx++){
        Triangle t = (*this->triangles)[idx];
        glm::vec3 centroid = t.centroid();
        for (int axis = 0; axis < 3; axis++){
            float i = (centroid[axis] - parent->bbox.min[axis])/bbox_side_lengths[axis];
            i = std::max(0.f, std::min(i, .999f));
            int bin_num = (int) floor(this->n_bins * i);
            Bin& bin = this->bins[axis][bin_num];
            bin.n += 1;
            bin.bbox = BBox::unionBBox(t.bbox(), bin.bbox);
        }
    }

    //calculate splits
    for (int axis = 0; axis < 3; axis++){

        BBox left_bbox;
        BBox right_bbox;
        int left_n = 0;
        int right_n = 0;

        for (int i = 0; i < this->n_bins-1; i++){
            Bin& bin = this->bins[axis][i];
            Split& split = this->splits[axis][i];
            left_bbox = BBox::unionBBox(left_bbox, bin.bbox);
            left_n += bin.n;
            split.left_bbox = left_bbox;
            split.left_n = left_n;
        }

        for (int i = this->n_bins-2; i >= 0; i--){
            Bin& bin = this->bins[axis][i + 1];
            Split& split = this->splits[axis][i];
            right_bbox = BBox::unionBBox(right_bbox, bin.bbox);
            right_n += bin.n;
            split.right_bbox = right_bbox;
            split.right_n = right_n;
        }
    }

    //find min cost split
    float min_cost = std::numeric_limits<float>::infinity();
    int min_axis = 0;
    int min_split_idx;
    Split min_split;
  
    //compute costs for each split
    for (int axis = 0; axis < 3; axis++){
        for (int i = 0; i < this->n_bins - 1; i++){

            Split& split = this->splits[axis][i];

            float p_right = split.right_bbox.surface_area()/parent->bbox.surface_area();
            float p_left = split.left_bbox.surface_area()/parent->bbox.surface_area();
            float cost = p_left * split.left_n + p_right * split.right_n;

            if (cost < min_cost && split.left_n > 0 && split.right_n > 0 && cost > 0.f){
                min_cost = cost;
                min_axis = axis;
                min_split_idx = i;
                min_split = split;
            }
        }
    }

    if (parent->n <= min_cost){
        return;
    }

    float bin_size = bbox_side_lengths[min_axis]/this->n_bins;
    float partition_cord = parent->bbox.min[min_axis] + bin_size * (min_split_idx + 1);

    auto it = std::partition(this->triangles->begin() + parent->offset, 
                   this->triangles->begin() + parent->offset + parent->n,
                   [min_axis, partition_cord](Triangle &t){return t.centroid()[min_axis] <= partition_cord;});

    int partition_index = (it - this->triangles->begin());

    BVHNode* left = new BVHNode();
    BVHNode* right = new BVHNode();

    left->offset = parent->offset;
    left->n = partition_index - parent->offset;
    left->bbox = min_split.left_bbox;

    right->offset = partition_index;
    right->n = parent->n - left->n;
    right->bbox = min_split.right_bbox;

    parent->right = right;
    parent->left = left;

    

    this->buildRecursive(left);
    this->buildRecursive(right);
    
};



#endif