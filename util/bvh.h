#ifndef BVH_H
#define BVH_H
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include <algorithm>
class bvh_node :public hittable{
public:
    bool hit(const Ray& r,interval ray_t,hit_record& rec) const override{
        if(!bbox.hit(r,ray_t)){
            return false;
        }
        auto hit_left = left->hit(r,ray_t,rec);
        auto hit_right = right->hit(r,interval(ray_t.min,hit_left ? rec.t : ray_t.max),rec);
        return hit_left || hit_right;
    }
    aabb bounding_box() const override{
        return bbox;
    }
    bvh_node(std::vector<shared_ptr<hittable>>& objects,size_t start,size_t end){
        bbox = aabb::empty;
        for(size_t index=start;index<=end;index++){
            bbox = aabb(bbox,objects[index]->bounding_box());
        }
        int axis = bbox.longest_axis();
        auto comparator = (axis==0) ? box_x_compare :
                          (axis==1) ? box_y_compare :
                                      box_z_compare ;
        int len =end -start;
        if(len == 0){
            left = right = objects[start];
        }
        else if(len==1){
            std::sort(objects.begin()+start,objects.begin()+end+1,comparator);
            left = objects[start];
            right = objects[start+1];
        }
        else{
            std::sort(objects.begin()+start,objects.begin()+end+1,comparator);
            int mid = start + len/2;
            left = make_shared<bvh_node>(objects,start,mid);
            right = make_shared<bvh_node>(objects,mid+1,end);
        }
        bbox = aabb(left->bounding_box(),right->bounding_box());
    }
    bvh_node(hittable_list list) : bvh_node (list.objects,0,list.objects.size()-1) {}
private:
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;
    aabb bbox;
    static bool box_compare(
        const shared_ptr<hittable> a,const shared_ptr<hittable> b,int axis_index
    ){
        auto a_axis_interval = a->bounding_box().axis_interval(axis_index);
        auto b_axis_interval = b->bounding_box().axis_interval(axis_index);
        return a_axis_interval.min < b_axis_interval.min;
    }
    static bool box_x_compare(
        const shared_ptr<hittable> a,const shared_ptr<hittable> b
    ){
        return box_compare(a,b,0);
    }
    static bool box_y_compare(
        const shared_ptr<hittable> a,const shared_ptr<hittable> b
    ){
        return box_compare(a,b,1);
    }
    static bool box_z_compare(
        const shared_ptr<hittable> a,const shared_ptr<hittable> b
    ){
        return box_compare(a,b,2);
    }
};
#endif