#ifndef SPHERE_H
#define SPHERE_H
#include "hittable.h"
#include "rtweekend.h"
class sphere : public hittable{
public:
    sphere(const Point3& center,double radius,shared_ptr<material> mat) 
    : center1(center),radius(std::fmax(0.0,radius)),
    mat(mat),is_moving(false)
    {
        auto rvec = vec3(radius,radius,radius);
        bbox=aabb(center1 - rvec,center1 + rvec);
    }
    sphere(const Point3& center,const Point3& center_to, double radius,shared_ptr<material>mat):
    center1(center),radius(std::fmax(0.0,radius)),mat(mat),is_moving(true)
    {
        auto rvec = vec3(radius,radius,radius);
        aabb bbox1 = aabb(center1 - rvec,center1 + rvec);
        aabb bbox2 = aabb(center_to - rvec ,center_to + rvec);
        bbox = aabb(bbox1,bbox2);
        center_vec = center_to - center;
    }
    aabb bounding_box() const override{
        return bbox;
    }
    bool hit(const Ray& r,interval ray_t,hit_record& rec) const override{
        Point3 center = is_moving? sphere_center(r.time()) : center1;
        vec3 oc=center-r.origin();
        double a=r.direction().length_squared();
        double h= dot(r.direction(),oc);
        double c= oc.length_squared() - radius*radius;
        double discriminant= h * h - a * c;
        if(discriminant<0.0) return false;
        double sqrtd=std::sqrt(discriminant);
        double root=(h-sqrtd)/a;
        if(!ray_t.surrounds(root)){
            root=(h+sqrtd)/a;
            if(!ray_t.surrounds(root)){
                return false;
            }
        }
        rec.t=root;
        rec.p=r.at(root);
        vec3 outwrad_normal=unit_vector(rec.p-center);
        rec.set_face_normal(r,outwrad_normal);
        rec.mat=mat;
        get_sphere_uv(outwrad_normal,rec.u,rec.v);
        return true;
    }
    static void get_sphere_uv(const Point3& p,double &u , double& v){
        auto theta = std::acos(-p.y());
        auto phi = std::atan2(-p.z(),p.x())+pi;
        u = phi / (2 *pi);
        v = theta / pi;
    }
private:
    bool is_moving;
    vec3 center_vec;
    Point3 center1;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;
    Point3 sphere_center(double time) const{
        return center1 + time*center_vec;
    }
};
#endif