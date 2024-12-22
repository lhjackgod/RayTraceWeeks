#ifndef AABB_H
#define AABB_H
#include "rtweekend.h"

class aabb{
public:
    interval x,y,z;
    aabb(){}
    aabb(const interval& x,const interval& y,const interval& z):
    x(x),y(y),z(z){ pad_to_minimums(); }
    aabb(const Point3& a,const Point3& b){
        x = a.x() < b.x() ? interval(a.x(),b.x()) : interval(b.x(),a.x());
        y = a.y() < b.y() ? interval(a.y(),b.y()) : interval(b.y(),a.y());
        z = a.z() < b.z() ? interval(a.z(),b.z()) : interval(b.z(),a.z());
        pad_to_minimums();
    }
    aabb(const aabb& a,const aabb& b){
        x = interval(a.x,b.x);
        y = interval(a.y,b.y);
        z = interval(a.z,b.z);
    }
    const interval& axis_interval(int n) const{
        if(n == 1) return y;
        if(n == 2) return z;
        return x;
    }
    bool hit(const Ray& r , interval ray_t) const{
        const Point3& ray_ori = r.origin();
        const vec3& ray_dir = r.direction();
         for(int i=0;i<3;i++){
            const interval& ax = axis_interval(i);
            const double& adinv = 1.0 / ray_dir[i];
            auto t0 = (ax.min - ray_ori[i]) * adinv;
            auto t1 = (ax.max - ray_ori[i]) * adinv;
            if( t0 < t1 ){
                ray_t.min = t0 > ray_t.min ? t0 : ray_t.min;
                ray_t.max = t1 < ray_t.max ? t1 : ray_t.max;
            }
            else{
                ray_t.min = t1 > ray_t.min ? t1 : ray_t.min;
                ray_t.max = t0 < ray_t.max ? t0 : ray_t.max;
            }
            if(ray_t.min >= ray_t.max) {
                return false;
            }
         }
         return true;
    }
    int longest_axis() const {
        return x.size() > y.size() ? (x.size() > z.size() ? 0 : 2) : (y.size() > z.size() ? 1 : 2); 
    }
    static const aabb empty,universe;
private:
    void pad_to_minimums(){
        double delta = 0.0001;
        if(x.size() < delta) x = x.expand(delta);
        if(y.size() < delta) y = y.expand(delta);
        if(z.size() < delta) z = z.expand(delta);
    }
};
const aabb aabb::empty = aabb(interval::empty , interval::empty , interval::empty);
const aabb aabb::universe = aabb(interval::universe , interval::universe,interval::universe);
aabb operator + (const aabb& bbox,const vec3& offset){
    return aabb(bbox.x+offset.x(),bbox.y+offset.y(),bbox.z+offset.z());
}
aabb operator + (const vec3& offset,const aabb& bbox){
    return bbox + offset;
}
#endif