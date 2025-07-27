#ifndef VECMATH_H
#define VECMATH_H
#include <limits.h>

template<template <typename> class Child, typename T>
class Tuple2
{
public:
    static const int nDimensions = 2;
    Tuple2() = default;
    Tuple2(T x, T y) : x(x), y(y) {}

    template<typename U>
    auto operator+(Child<U> c) const -> Child<decltype(T{} + U{})>
    {
        return {x + c.x, y + c.y};
    }
    template<typename U>
    Child<T> &operator+=(Child<U> c)
    {
        x += c.x;
        y += c.y;
        return static_cast<Child<T> &>(*this);
    }
    template<typename U>
    auto operator-(Child<U> c) const -> Child<decltype(T{} - U{})>
    {
        return {x - c.x, y - c.y};
    }
    template<typename U>
    Child<T> &operator-=(Child<U> c)
    {
        x -= c.x;
        y -= c.y;
        return static_cast<Child<T> &>(*this);
    }
    bool operator==(Child<T> c) const { return x == c.x && y == c.y; }
    bool operator!=(Child<T> c) const { return x != c.x || y != c.y; }
    
    template<typename U>
    auto operator*(U s) const -> Child<decltype(T{} * U{})> {
        return {s * x, s * y};
    }

    template<typename U>
    Child<T> &operator*=(U s) {
        x *= s;
        y *= s;
        return static_cast<Child<T> &>(*this);
    }

    template<typename U>
    auto operator/(U d) const -> Child<decltype(T{} / U{})> {
        return {x / d, y / d};
    }
    template <typename U>
    Child<T> &operator/=(U d) {
        x /= d;
        y /= d;
        return static_cast<Child<T> &>(*this);
    }

    Child<T> operator-() const { return {-x, -y}; }

    T operator[](int i) const {
        return (i == 0) ? x : y;
    }

    T &operator[](int i) {
        return (i == 0) ? x : y;
    }
    T x{}, y{};
};

template<template <typename> class C, typename T>
inline C<T> Min(Tuple2<C, T> p1, Tuple2<C, T> p2)
{
    using std::min;
    return {min(p1.x, p2.x), min(p1.y, p2.y)};
}

template<template <typename> class C, typename T>
inline C<T> Max(Tuple2<C, T> p1, Tuple2<C, T> p2)
{
    using std::max;
    return {max(p1.x, p2.x), max(p1.y, p2.y)};
}

template<typename T>
class Point2;

template<typename T>
class Vector2 : public Tuple2<Vector2, T>
{
    public:
    using Tuple2<Vector2, T>::x;
    using Tuple2<Vector2, T>::y;
    using Tuple2<Vector2, T>::operator+;
    using Tuple2<Vector2, T>::operator+=;
    using Tuple2<Vector2, T>::operator*;
    using Tuple2<Vector2, T>::operator*=;
    using Tuple2<Vector2, T>::operator-;
    Vector2() {x = y = 0;}
    Vector2(T x, T y) : Tuple2<Vector2, T>(x, y) {}
    Vector2(Point2<T> p) : Tuple2<Vector2, T>(p.x, p.y) {}
};

typedef Vector2<int> Vector2i;

template <typename T>
class Point2 : public Tuple2<Point2, T>
{
    public:
    using Tuple2<Point2, T>::x;
    using Tuple2<Point2, T>::y;
    using Tuple2<Point2, T>::operator+;
    using Tuple2<Point2, T>::operator+=;
    using Tuple2<Point2, T>::operator*;
    using Tuple2<Point2, T>::operator*=;
    Point2() {x = y = 0;}
    Point2(T x, T y) : Tuple2<Point2, T>(x, y) {}
    Point2(Vector2<T> v) : Tuple2<Point2, T>(v.x, v.y) {}
    template<typename U>
    Point2<T> operator+(Vector2<U> v)
    {
        return {x + v.x, y + v.y};
    }
};

typedef Point2<int> Point2i;

template<typename T>
class Bounds2
{
public:
    Bounds2()
    {
        T minNum = std::numeric_limits<T>::min();
        T maxNum = std::numeric_limits<T>::max();
        pMin = Point2<T>(maxNum, maxNum);
        pMax = Point2<T>(minNum, minNum);
    }
    Bounds2(Point2<T> p1, Point2<T> p2)
     : pMin(Min(p1, p2)), pMax(Max(p1, p2))
    {

    }
    Bounds2(T x1, T y1, T x2, T y2)
    : pMin(Min(Point2<T>(x1, y1), Point2<T>(x2, y2))), pMax(Max(Point2<T>(x1, y1), Point2<T>(x2, y2)))
    {

    }
    bool IsEmpty() const { return pMin.x >= pMax.x || pMin.y >= pMax.y; }
    T Area() const 
    {
        Vector2<T> d = pMax - pMin;
        return d.x * d.y;
    }
    bool operator==(const Bounds2<T> &b) const {
        return b.pMin == pMin && b.pMax == pMax;
    }
    bool operator!=(const Bounds2<T> &b) const {
        return b.pMin != pMin || b.pMax != pMax;
    }
    Vector2<T> Diagonal() const { return pMax - pMin; }
public:
    Point2<T> pMin;
    Point2<T> pMax;
};

template<typename T>
inline Bounds2<T> Intersect(Bounds2<T> b1, Bounds2<T> b2)
{
    return Bounds2<T>(Max(b1.pMin, b2.pMin), Min(b1.pMax, b2.pMax));
}

typedef Bounds2<int>Bounds2i;

class Bounds2iIterator : public std::forward_iterator_tag
{
public:
    Bounds2iIterator(const Bounds2i& b, const Point2i& p)
      : bounds(&b), p(p){}
    Bounds2iIterator operator++()
    {
        advance();
        return *this;
    }
    bool operator!=(const Bounds2iIterator& other) const
    {
        return p != other.p || bounds != other.bounds;
    }
    bool operator==(const Bounds2iIterator& other) const
    {
        return p == other.p && bounds == other.bounds;
    }
    Point2i operator*()
    {
        return p;
    }
private:
    void advance()
    {
        ++p.x;
        if(p.x == bounds->pMax.x)
        {
            p.x = bounds->pMin.x;
            ++p.y;
        }
    }
    Point2i p;
    const Bounds2i* bounds;
};

inline Bounds2iIterator begin(const Bounds2i& b)
{
    return Bounds2iIterator(b, b.pMin);
}

inline Bounds2iIterator end(const Bounds2i& b)
{
    Point2i pEnd(b.pMin.x, b.pMax.y);
    if(b.pMin.x >= b.pMax.x || b.pMin.y >= b.pMax.y)
    {
        pEnd = b.pMin;
    }
    return Bounds2iIterator(b, pEnd);
}

#endif