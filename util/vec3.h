#ifndef VEC3_H
#define VEC3_H
#include "rtweekend.h"
class vec3{
public:
    double e[3];
    
    vec3() : e{0,0,0}{}
    vec3(double a,double b,double c):e{a,b,c}{}
    double x() const {return e[0];}
    double y() const {return e[1];}
    double z() const {return e[2];}
    vec3 operator-() const {return vec3(-e[0],-e[1],-e[2]);}
    double operator[](int i) const{return e[i];}
    double& operator[](int i) {return e[i];}
    vec3& operator+=(const vec3& v){
        e[0]+=v.e[0];
        e[1]+=v.e[1];
        e[2]+=v.e[2];
        return *this;
    }
    vec3& operator*=(const double& t){
        e[0]*=t;
        e[1]*=t;
        e[2]*=t;
        return *this;
    }
    vec3& operator/=(const double& t){
        e[0]/=t;
        e[1]/=t;
        e[2]/=t;
        return *this;
    }
    auto length_squared() const{
        return e[0]*e[0]+e[1]*e[1]+e[2]*e[2];
    }
    auto length()const{
        return std::sqrt(length_squared());
    }
    bool near_zero() const{
        double s = 1e-8;
        return (std::fabs(e[0])<s) && (std::fabs(e[1]) < s) && (std::fabs(e[2]) < s);
    }
    static vec3 random(){
        return vec3(random_double() , random_double() , random_double());
    }
    static vec3 random(double min , double max){
        return vec3(random_double(min,max) , random_double(min,max) , random_double(min,max));
    }
};
using Point3=vec3;
inline std::ostream& operator<<(std::ostream& out,const vec3& v){
    return out<<v.e[0]<<" "<<v.e[1]<<" "<<v.e[2];
}
inline vec3 operator+(const vec3& u,const vec3& v){
    return vec3(u[0]+v[0],u[1]+v[1],u[2]+v[2]);
}
inline vec3 operator-(const vec3& u,const vec3& v){
    return vec3(u[0]-v[0],u[1]-v[1],u[2]-v[2]);
}
inline vec3 operator*(const vec3& u,const vec3& v){
    return vec3(u[0]*v[0],u[1]*v[1],u[2]*v[2]);
}
inline vec3 operator*(const vec3& u,double v){
    return vec3(u[0]*v,u[1]*v,u[2]*v);
}
inline vec3 operator*(double v,const vec3& u){
    return u*v;
}
inline vec3 operator/(const vec3& u,double t){
    return (1/t)*u;
}
inline double dot(const vec3& u,const vec3& v){
    return u.e[0]*v.e[0]+u.e[1]*v.e[1]+u.e[2]*v.e[2];
}
inline vec3 cross(const vec3& u,const vec3& v){
    return vec3(u.e[1]*v.e[2]-u.e[2]*v.e[1],
    u.e[2]*v.e[0]-u.e[0]*v.e[2],
    u.e[0]*v.e[1]-u.e[1]*v.e[0]);
}
inline vec3 unit_vector(const vec3& v){
    return v/v.length();
}
inline vec3 random_in_unit_sphere(){
    while(true){
        auto p =vec3::random(-1,1);
        if(p.length_squared() < 1){
            return p;
        }
    }
}
inline vec3 random_unit_vector(){
    return unit_vector(random_in_unit_sphere());
}
inline vec3 random_on_hemisphere(const vec3& normal){
    vec3 on_unit_sphere = random_unit_vector();
    if(dot(normal,on_unit_sphere) > 0.0){
        return on_unit_sphere;
    }
    else{
        return -on_unit_sphere;
    }
}
inline vec3 reflect(const vec3& v, const vec3& n ){
    vec3 b = -dot(v,n) * n;
    vec3 direction = v + 2*b;
    direction = unit_vector(direction);
    return direction;
}
inline vec3 refract(const vec3& uv,const vec3& n,double etai_over_etat){
    double cos_theta = std::fmin(-dot(uv,n),1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -std::sqrt(std::fabs(1.0-r_out_perp.length_squared())) * n;
    return unit_vector(r_out_parallel + r_out_perp);
}
inline vec3 random_in_unit_disk(){
    while(true){
        vec3 p = vec3(random_double(-1,1),random_double(-1,1),0.0);
        if(p.length() < 1.0){
            return p;
        }
    }
}
#endif