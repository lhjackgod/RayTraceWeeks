#ifndef RTWEEKEND_H
#define RTWEEKEND_H
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cmath>
using std::shared_ptr;
using std::make_shared;

const double infinity=std::numeric_limits<double>::infinity();
const double pi=3.1415926535897932385;

inline double degrees_to_radians(double degree){
    return degree * pi / 180.0;
}
inline double random_double(){
    return std::rand() / (RAND_MAX + 1.0);
}
inline double random_double(double min,double max){
    return min + (max-min) * random_double();
}
inline int random_int(int min,int max){
    return static_cast<int>(random_double(min,max+1));
}
#include "vec3.h"
#include "interval.h"
#include "color.h"
#include "ray.h"
#endif