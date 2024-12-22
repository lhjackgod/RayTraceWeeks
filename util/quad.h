#ifndef QUAD_H
#define QUAD_H
#include "rtweekend.h"
#include "hittable.h"
class quad : public hittable{
public:
    quad(const Point3& Q,const vec3& u,const vec3& v,shared_ptr<material> mat) : Q(Q),u(u),v(v),mat(mat) {
        auto n = cross(u,v);
        normal = unit_vector(n);
        D = dot(Q,normal);
        w = n/ dot(n,n);
        set_bounding_box();
    }
    virtual void set_bounding_box(){
        auto box_diagonal1 = aabb(Q,Q+u+v);
        auto box_diagonal2 = aabb(Q+v,Q+u);
        bbox = aabb(box_diagonal1,box_diagonal2);
    }
    bool hit(const Ray& r,interval ray_t,hit_record& rec) const override{
        double denom = dot(normal,r.direction());
        if(std::fabs(denom) < 1e-8){
            return false;
        }
        auto t = (D - dot(normal,r.origin())) / denom;
        if(!ray_t.contains(t)){
            return false;
        }
        Point3 intersection = r.at(t);
        // Determine if the hit point lies within the planar shape using its plane coordinates.
        vec3 planar_hitpt_vector = intersection - Q;//这是平面交点和四边形原点的向量
        double alpha = dot(w , cross(planar_hitpt_vector,v));
        double beta = dot(w , cross(u,planar_hitpt_vector));
        if(!is_interior(alpha,beta,rec)){
            return false;
        }
        rec.p = intersection;
        rec.t = t;
        rec.mat = mat;
        rec.set_face_normal(r,normal);
        return true;
    }
    virtual bool is_interior(double alpha,double beta,hit_record& rec) const{
        interval unit_interval = interval(0,1);
        if(!unit_interval.contains(alpha) || !unit_interval.contains(beta)){
            return false;
        }
        rec.u = alpha;
        rec.v = beta;
        return true;
    }
    aabb bounding_box() const override{
        return bbox;
    }
private:
    Point3 Q;
    vec3 u,v;
    vec3 normal;
    vec3 w;
    double D;
    shared_ptr<material> mat;
    aabb bbox;
};
inline shared_ptr<hittable_list> box(const Point3& a,const Point3& b,shared_ptr<material> mat){
    auto sides = make_shared<hittable_list>();
    auto min = Point3(std::fmin(a.x(),b.x()),std::fmin(a.y(),b.y()),std::min(a.z(),b.z()));
    auto max = Point3(std::fmax(a.x(),b.x()),std::fmax(a.y(),b.y()),std::fmax(a.z(),b.z()));

    auto dx = vec3(max.x() - min.x() , 0 , 0 );
    auto dy = vec3(0 , max.y() - min.y() , 0 );
    auto dz = vec3(0 , 0 , max.z() - min.z());

    sides->add(make_shared<quad>(Point3(min.x(),min.y(),max.z()),dx,dy,mat)); // front
    sides->add(make_shared<quad>(Point3(min.x(),min.y(),min.z()),dz,dy,mat)); //left
    sides->add(make_shared<quad>(Point3(max.x(),min.y(),min.z()),-dx,dy,mat));//back
    sides->add(make_shared<quad>(Point3(max.x(),min.y(),max.z()),-dz,dy,mat));//right
    sides->add(make_shared<quad>(Point3(min.x(),max.y(),max.z()),dx,-dz,mat));//up
    sides->add(make_shared<quad>(Point3(min.x(),min.y(),min.z()),dx,dz,mat)); //bottom
    return sides;
}
#endif