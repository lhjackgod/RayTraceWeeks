#ifndef HITTABLE_H
#define HITTABLE_H
#include "rtweekend.h"
#include "aabb.h"
class material;
class hit_record{
public:
    Point3 p;
    double t;
    vec3 normal;
    bool front_face;
    shared_ptr<material> mat;
    double u;
    double v;
    void set_face_normal(const Ray& r,const vec3 & outwrad_normal){
        front_face = dot(r.direction(),outwrad_normal) < 0.0;
        normal = front_face? outwrad_normal: - outwrad_normal;
    }
};
class hittable{
public:
    virtual ~hittable() = default;
    virtual bool hit(const Ray& r,interval ray_t,hit_record& rec) const =0;
    virtual aabb bounding_box() const =0;
    virtual double pdf_value(const Point3& origin, const vec3& direction) const 
    {
        return 0.0;
    }
    virtual vec3 random(const vec3& origin) const{
        return vec3(1, 0, 0);
    }
};
class translate : public hittable{
public:
    translate(shared_ptr<hittable> object,const vec3& offset) : object(object),offset(offset){
        bbox = object->bounding_box() + offset;
    }
    bool hit(const Ray& r,interval ray_t,hit_record& rec) const override{
        Ray offset_r(r.origin()-offset,r.direction(),r.time());
        if(!object->hit(offset_r,ray_t,rec)){
            return false;
        }
        rec.p+=offset;
        return true;
    }
    aabb bounding_box() const {
        return bbox;
    }
private:
    shared_ptr<hittable> object;
    vec3 offset;
    aabb bbox;
};
class rotate_y : public hittable{
public:
    rotate_y(shared_ptr<hittable> object,double angle) : object(object){
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        vec3 min(infinity,infinity,infinity);
        vec3 max(-infinity,-infinity,-infinity);
        for(int i=0;i<2;i++){
            for(int j=0;j<2;j++){
                for(int k=0;k<2;k++){
                    auto x = bbox.x.min * i + bbox.x.max * (1-i);
                    auto y = bbox.y.min * j + bbox.y.max * (1-j);
                    auto z = bbox.z.min * k + bbox.z.max * (1-k);

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;
                    vec3 tester(newx,y,newz);
                    for(int c =0 ; c < 3 ; c++){
                        min[c] = std::fmin(min[c] , tester[c]);
                        max[c] = std::fmax(max[c] , tester[c]);
                    }
                }
            }
        }
        bbox = aabb(min,max);
    }
    bool hit(const Ray& r,interval ray_t,hit_record &rec) const override{
        Point3 origin = r.origin();
        vec3 direction = r.direction();
        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];

        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];
        Ray rotated_r = Ray(origin,direction,r.time());

        if(!object->hit(rotated_r,ray_t,rec)) {return false;}
        auto p = rec.p;
        p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
        p[2] = - sin_theta * rec.p[0] + cos_theta * rec.p[2];
        
        auto normal = rec.normal;
        normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
        normal[2] = - sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

        rec.p = p;
        rec.normal = unit_vector(normal);
        return true;
    }
    aabb bounding_box() const override{ return bbox; }
private:
    shared_ptr<hittable> object;
    double sin_theta;
    double cos_theta;
    aabb bbox;
};
#endif