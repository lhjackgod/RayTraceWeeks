#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H
#include "aabb.h"
#include "hittable.h"
#include "rtweekend.h"
#include <vector>
class hittable_list : public hittable{
public:
    std::vector<shared_ptr<hittable>> objects;
    void add(shared_ptr<hittable> object){
        objects.push_back(object);
        bbox = aabb(bbox,object->bounding_box());
    }
    void clear(){ objects.clear(); }
    hittable_list(){}
    hittable_list(shared_ptr<hittable> object){ add(object); }
    bool hit(const Ray& r,interval ray_t,hit_record& rec) const override{
        hit_record temp_rec;
        bool hit_anything = false;
        double closet_so_far = ray_t.max;
        for(const shared_ptr<hittable>& object:objects){
            if(object->hit(r,interval(ray_t.min,closet_so_far),temp_rec)){
                hit_anything=true;
                closet_so_far=temp_rec.t;
                rec=temp_rec;
            }
        }
        return hit_anything;
    }
    aabb bounding_box() const override{
        return bbox;
    }
private:
    aabb bbox;
};
#endif