#ifndef RAY_H
#define RAY_H
#include "vec3.h"
class Ray{
public:
    Ray(){}
    Ray(const Point3& origin,const vec3& direction,double time):
    orig(origin),dir(direction),tm(time){}
    Ray(const Point3& origin,const vec3& direction) : orig(origin),dir(direction),tm(0){}
    const Point3& origin() const {return orig;}
    const vec3& direction() const {return dir;}
    Point3 at(double t) const {
        return orig+t*dir;
    }
    double time() const {return tm;}
private:
    Point3 orig;
    vec3 dir;
    double tm;
};
#endif