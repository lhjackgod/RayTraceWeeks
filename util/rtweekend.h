#ifndef RTWEEKEND_H
#define RTWEEKEND_H
#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <cmath>
#include <span>
#include <map>
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

#define CHECK_EQ(a, b) CHECK_IMPL(a, b, ==)
#define CHECK_NE(a, b) CHECK_IMPL(a, b, !=)
#define CHECK_GT(a, b) CHECK_IMPL(a, b, >)
#define CHECK_GE(a, b) CHECK_IMPL(a, b, >=)
#define CHECK_LT(a, b) CHECK_IMPL(a, b, <)
#define CHECK_LE(a, b) CHECK_IMPL(a, b, <=)

#define CHECK_IMPL(a, b, op)                                      \
do                                                                \
{                                                                 \
    auto va = a;                                                  \
    auto vb = b;                                                  \
    if(!(va op vb))                                               \
    {                                                             \
        fprintf(stderr, "Check failed: %s %s %s\n", #a, #op, #b); \
        exit(EXIT_FAILURE);                                       \
    }                                                             \
}while(false)


namespace rtw{

template<typename... Ts>
struct TypePack
{
    static constexpr size_t count = sizeof...(Ts);
};

template <typename T, typename... Ts>
struct IndexOf
{
    static constexpr int count = 0;
    static_assert(!std::is_same_v<T, T>, "Type not present in TypePack");
};

template<typename T, typename... Ts>
struct IndexOf<T, TypePack<T, Ts...>>
{
    static constexpr int count = 0;
};


template <typename T, typename U, typename... Ts>
struct IndexOf<T, TypePack<U, Ts...>>
{
    static constexpr int count = 1 + IndexOf<T, TypePack<Ts...>>::count;
};

}
#include "vec3.h"
#include "interval.h"
#include "ray.h"
#endif