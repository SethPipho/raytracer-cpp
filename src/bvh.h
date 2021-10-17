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


class BVH {

    struct BVHBin {
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
        std::vector<Triangle> triangles;
        BVHNode* root;
        int n_bins = 64;
        std::vector<BVHBin> bins[3];
        std::vector<Split> splits[3];

        BVH(){
            for (int i = 0; i < 3; i++){
                this->bins[i] = std::vector<BVHBin>(this->n_bins, BVHBin());
                this->splits[i] = std::vector<Split>(this->n_bins - 1, Split());
            }
        }

        ~BVH(){
            this->free_nodes(this->root);
        }

        void free_nodes(BVHNode* parent){
            if (parent->left == nullptr){
                return;
            }
            this->free_nodes(parent->left);
            this->free_nodes(parent->right);
            delete parent;
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
    if (t > nearest->t){
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

    if (parent->n < 8){
        return;
    }

   for (int i = 0; i < 3; i++){
        this->bins[i] = std::vector<BVHBin>(this->n_bins, BVHBin());
        this->splits[i] = std::vector<Split>(this->n_bins - 1, Split());
    }


    glm::vec3 bbox_side_lengths = parent->bbox.max - parent->bbox.min;

    //bin triangles by centroid
    for (int idx = parent->offset; idx < parent->offset + parent->n; idx++){
        Triangle t = this->triangles[idx];
        glm::vec3 centroid = t.centroid();
        for (int axis = 0; axis < 3; axis++){
            //TODO out of bounds error
            float i = (centroid[axis] - parent->bbox.min[axis])/bbox_side_lengths[axis];
            i = std::max(0.f, std::min(i, .9999999f));
            int bin_num = (int)(this->n_bins * i);
            //assert (bin_num < this->n_bins);
            //assert (bin_num >= 0);
            BVHBin& bin = this->bins[axis][bin_num];
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
            BVHBin& bin = this->bins[axis][i];
            Split& split = this->splits[axis][i];
            left_bbox = BBox::unionBBox(left_bbox, bin.bbox);
            left_n += bin.n;
            split.left_bbox = left_bbox;
            split.left_n = left_n;
        }

        for (int i = this->n_bins-2; i >= 0; i--){
            BVHBin& bin = this->bins[axis][i];
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
    float partition_cord = parent->bbox.min[min_axis] + bin_size * min_split_idx;

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
    left->bbox = min_split.left_bbox;

    right->offset = partition_index;
    right->n = parent->n - left->n;
    right->bbox = min_split.right_bbox;

  

   if (left->n < 1 || right->n < 1){
      return;
    }

    parent->right = right;
    parent->left = left;

    this->buildRecursive(left);
    this->buildRecursive(right);
    
};



#endif