#ifndef VECMATH_H
#define VECMATH_H
#include <limits.h>
#include "rtweekend.h"
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
    std::string ToString() const
    {
        return std::format("({}, {})", x, y);
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
typedef Point2<float> Point2f;

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

template<int n>
inline constexpr float Pow(float v)
{
    if constexpr (n < 0)
    {
        return 1.0f / Pow<-n>(v);
    }
    float n2 = Pow<n / 2>(v);
    return n2 * n2 * Pow<n & 1>(v);
}

template<>
inline constexpr float Pow<1>(float v)
{
    return v;
}

template<>
inline constexpr float Pow<0>(float v)
{
    return 1.f;
}

template<typename T, typename U, typename V>
inline constexpr T Clamp(T val, U min, V max)
{
    if(val < min)      return (T)min;
    else if(val > max) return (T)max;
    return val;
}

template<typename Predicate>
inline size_t FindInterval(size_t size, const Predicate& pred)
{
    size_t l = 0, r = size - 1;
    while(l < r)
    {
        size_t mid = (l + r + 1) / 2;
        if(pred(mid)) l = mid;
        else r = mid - 1;
    }
    return Clamp(l, 0, size - 1);
}

inline float Lerp(float t, float a, float b)
{
    return a * (1 - t) + b * t;
}

template<int N>
inline void init(float m[N][N], int i, int j) {}

template<int N, typename... Args>
inline void init(float m[N][N], int i, int j, float v, Args... args)
{
    m[i][j] = v;
    if(++j == N)
    {
        ++i;
        j = 0;
    }
    init<N>(m, i, j, args...);
}

template<int N>
inline void initDiag(float m[N][N], int i) {}

template<int N, typename... Args>
inline void initDiag(float m[N][N], int i, float v, Args... args)
{
    m[i][i] = v;
    initDiag<N>(m, i + 1, args...);
}

template<typename T>
inline std::enable_if_t<std::is_integral_v<T>, T> FMA(T a, T b, T c)
{
    return a * b + c;
}

inline float FMA(float a, float b, float c)
{
    return std::fma(a, b, c);
}

inline double FMA(double a, double b, double c)
{
    return std::fma(a, b, c);
}

inline long double FMA(long double a, long double b, long double c)
{
    return std::fma(a, b, c);
}

template<typename Ta, typename Tb, typename Tc, typename Td>
inline auto DifferenceOfProducts(Ta a, Tb b, Tc c, Td d)
{
    auto cd = c * d;
    auto differenceOfProducts = FMA(a, b, cd);
    auto error = FMA(-c, d, cd);
    return differenceOfProducts + error;
}

template<int N>
class SquareMatrix 
{
public:
    static SquareMatrix Zero()
    {
        SquareMatrix m;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                m.m[i][j] = 0;
            }
        }
    }

    SquareMatrix()
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                m[i][j] = (i == j);
            }
        }
    }

    SquareMatrix(const float mat[N][N])
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                m[i][j] = mat[i][j];
            }
        }
    }

    SquareMatrix(const std::span<const float> t);

    template<typename... Args>
    SquareMatrix(float v, Args... args)
    {
        static_assert(1 + sizeof...(Args) == N * N, "Incorrect number of values provided to SquareMatrix constructor");
        init<N>(m, 0, 0, v, args...);
    }

    template<typename... Args>
    static SquareMatrix Diag(float v, Args... args)
    {
        static_assert(1 + sizeof...(Args) == N, "Incorrect number of values provided to SquareMatrix::Diag");
        SquareMatrix m;
        initDiag<N>(m.m, 0, v, args...);
        return m;
    }

    SquareMatrix operator + (const SquareMatrix& other) const
    {
        SquareMatrix r = *this;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                r.m[i][j] += other.m[i][j];
            }
        }
        return r;
    }

    SquareMatrix operator *(const float s) const 
    {
        SquareMatrix r = *this;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                r.m[i][j] *= s;
            }
        }
        return r;
    }

    SquareMatrix operator / (const float s) const
    {
        CHECK_NE(s, 0);
        SquareMatrix r = *this;
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                r.m[i][j] /= s;
            }
        }
        return r;
    }

    bool operator == (const SquareMatrix<N> &m2) const 
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                if(m[i][j] != m2.m[i][j]) return false;
            }
        }
        return true;
    }

    bool operator != (const SquareMatrix<N> &m2) const 
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                if(m[i][j] != m2.m[i][j]) return true;
            }
        }
        return false;
    }

    bool operator < (const SquareMatrix<N> &m2) const 
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                if(m[i][j] < m2.m[i][j]) return true;
                else if(m[i][j] > m2.m[i][j]) return false;
            }
        }
        return false;
    }

    bool IsIdentity() const
    {
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                if(i == j)
                {
                    if(m[i][j] != 1.f)
                        return false;
                }
                else if(m[i][j] != 0.f)
                    return false;
            }
        }
        return true;
    }

    std::string ToString() const
    {
        std::string s = "[ [";
        for(int i = 0; i < N; ++i)
        {
            for(int j = 0; j < N; ++j)
            {
                s += std::format(" {}", m[i][j]);
                if(j < N - 1)
                    s += ",";
                else 
                    s += " ]";   
            }
            if(i < N - 1)
                s += ", [";
        }
        s += " ]";
        return s;
    }

    std::span<const float> operator[](int i) const { return m[i]; }

    std::span<float> operator[](int i) { return std::span<float>(m[i]); }

private:
    float m[N][N];
};

template<int N>
inline SquareMatrix<N>::SquareMatrix(const std::span<const float> t)
{
    CHECK_EQ(N * N, t.size());
    for(int i = 0; i < N * N; ++i)
    {
        m[i / N][i % N] = t[i];
    }
}

struct CompensatedFloat
{
public:
    CompensatedFloat(float v, float err = 0) : v(v), err(err){}

    explicit operator float() const { return v + err; }
    explicit operator double() const { return static_cast<double>(v) + static_cast<double>(err); }

    std::string ToString() const;

    float v, err;
};

template<int N>
float Determinant(const SquareMatrix<N>& m);

template<>
inline float Determinant(const SquareMatrix<3>& m)
{
    float minor12 = DifferenceOfProducts(m[1][1], m[2][2], m[1][2], m[2][1]); //00
    float minor02 = DifferenceOfProducts(m[1][0], m[2][2], m[1][2], m[2][0]); //01
    float minor01 = DifferenceOfProducts(m[1][0], m[2][1], m[1][1], m[2][0]); //02

    return FMA(m[0][2], minor01, DifferenceOfProducts(m[0][0], minor12, m[0][1], minor02));
}

template<int N>
float Determinant(const SquareMatrix<N>& m)
{
    SquareMatrix<N - 1> sub;
    float det = 0;
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N - 1; ++j)
        {
            for(int k = 0; k < N - 1; ++k)
            {
                sub[j][k] = m[j + 1][k < i ? k : k + 1];
            }
        }
        float sign = (i & 1) ? -1 : 1;
        det += sign * m[0][i] * Determinant(sub);
    }
    return det;
}

inline CompensatedFloat TwoProd(float a, float b)
{
    float ab = a * b;
    return {ab, FMA(a, b, -ab)};
}

inline CompensatedFloat TwoSum(float a, float b)
{
    float s = a + b, delta = s - a;
    return {s, (a - (s - delta)) + (b - delta)};
}

namespace internal
{
    template<typename Float>
    inline CompensatedFloat InnerProduct(Float a, Float b)
    {
        return TwoProd(a, b);
    }

    template<typename Float, typename... T>
    inline CompensatedFloat InnerProduct(Float a, Float b, T... terms)
    {
        CompensatedFloat ab = TwoProd(a, b);
        CompensatedFloat tp = InnerProduct(terms...);
        CompensatedFloat sum = TwoSum(ab.v, tp.v);

        return {sum.v, ab.err + (tp.err + sum.err)};
    }

}

template<typename... T>
inline std::enable_if_t<std::conjunction_v<std::is_arithmetic<T>...>, float> InnerProduct(T... terms)
{
    CompensatedFloat ip = internal::InnerProduct(terms...);
    return float(ip);
}

template<int N>
std::optional<SquareMatrix<N>> Inverse(const SquareMatrix<N> &m);

template<>
inline std::optional<SquareMatrix<3>> Inverse(const SquareMatrix<3> &m)
{
    float det = Determinant(m);
    if(det == 0) return {};
    float invDet = 1.0f / det;

    SquareMatrix<3> r;
    r[0][0] = invDet * DifferenceOfProducts(m[1][1], m[2][2], m[1][2], m[2][1]);
    r[1][0] = invDet * DifferenceOfProducts(m[1][2], m[2][0], m[1][0], m[2][2]);
    r[2][0] = invDet * DifferenceOfProducts(m[1][0], m[2][1], m[1][1], m[2][0]);
    r[0][1] = invDet * DifferenceOfProducts(m[0][2], m[2][1], m[0][1], m[2][2]);
    r[1][1] = invDet * DifferenceOfProducts(m[0][0], m[2][2], m[0][2], m[2][0]);
    r[2][1] = invDet * DifferenceOfProducts(m[0][1], m[2][0], m[0][0], m[2][1]);
    r[0][2] = invDet * DifferenceOfProducts(m[0][1], m[1][2], m[0][2], m[1][1]);
    r[1][2] = invDet * DifferenceOfProducts(m[0][2], m[1][0], m[0][0], m[1][2]);
    r[2][2] = invDet * DifferenceOfProducts(m[0][0], m[1][1], m[0][1], m[1][0]);

    return r;
}

template<>
inline std::optional<SquareMatrix<4>> Inverse(const SquareMatrix<4> &m)
{
    // Via: https://github.com/google/ion/blob/master/ion/math/matrixutils.cc,
    // (c) Google, Apache license.

    // For 4x4 do not compute the adjugate as the transpose of the cofactor
    // matrix, because this results in extra work. Several calculations can be
    // shared across the sub-determinants.
    //
    // This approach is explained in David Eberly's Geometric Tools book,
    // excerpted here:
    //   http://www.geometrictools.com/Documentation/LaplaceExpansionTheorem.pdf
    float s0 = DifferenceOfProducts(m[0][0], m[1][1], m[1][0], m[0][1]);
    float s1 = DifferenceOfProducts(m[0][0], m[1][2], m[1][0], m[0][2]);
    float s2 = DifferenceOfProducts(m[0][0], m[1][3], m[1][0], m[0][3]);

    float s3 = DifferenceOfProducts(m[0][1], m[1][2], m[1][1], m[0][2]);
    float s4 = DifferenceOfProducts(m[0][1], m[1][3], m[1][1], m[0][3]);
    float s5 = DifferenceOfProducts(m[0][2], m[1][3], m[1][2], m[0][3]);

    float c0 = DifferenceOfProducts(m[2][0], m[3][1], m[3][0], m[2][1]);
    float c1 = DifferenceOfProducts(m[2][0], m[3][2], m[3][0], m[2][2]);
    float c2 = DifferenceOfProducts(m[2][0], m[3][3], m[3][0], m[2][3]);

    float c3 = DifferenceOfProducts(m[2][1], m[3][2], m[3][1], m[2][2]);
    float c4 = DifferenceOfProducts(m[2][1], m[3][3], m[3][1], m[2][3]);
    float c5 = DifferenceOfProducts(m[2][2], m[3][3], m[3][2], m[2][3]);

    float determinant = InnerProduct(s0, c5, -s1, c4, s2, c3, s3, c2, s5, c0, -s4, c1);
    if (determinant == 0)
        return {};
    float s = 1 / determinant;

    float inv[4][4] = {{s * InnerProduct(m[1][1], c5, m[1][3], c3, -m[1][2], c4),
                        s * InnerProduct(-m[0][1], c5, m[0][2], c4, -m[0][3], c3),
                        s * InnerProduct(m[3][1], s5, m[3][3], s3, -m[3][2], s4),
                        s * InnerProduct(-m[2][1], s5, m[2][2], s4, -m[2][3], s3)},

                       {s * InnerProduct(-m[1][0], c5, m[1][2], c2, -m[1][3], c1),
                        s * InnerProduct(m[0][0], c5, m[0][3], c1, -m[0][2], c2),
                        s * InnerProduct(-m[3][0], s5, m[3][2], s2, -m[3][3], s1),
                        s * InnerProduct(m[2][0], s5, m[2][3], s1, -m[2][2], s2)},

                       {s * InnerProduct(m[1][0], c4, m[1][3], c0, -m[1][1], c2),
                        s * InnerProduct(-m[0][0], c4, m[0][1], c2, -m[0][3], c0),
                        s * InnerProduct(m[3][0], s4, m[3][3], s0, -m[3][1], s2),
                        s * InnerProduct(-m[2][0], s4, m[2][1], s2, -m[2][3], s0)},

                       {s * InnerProduct(-m[1][0], c3, m[1][1], c1, -m[1][2], c0),
                        s * InnerProduct(m[0][0], c3, m[0][2], c0, -m[0][1], c1),
                        s * InnerProduct(-m[3][0], s3, m[3][1], s1, -m[3][2], s0),
                        s * InnerProduct(m[2][0], s3, m[2][2], s0, -m[2][1], s1)}};

    return SquareMatrix<4>(inv);
}

template<int N>
SquareMatrix<N> InvertOrExit(const SquareMatrix<N> &m)
{
    std::optional<SquareMatrix<N>> inv = Inverse(m);
    if(!inv.has_value())
    {
        fprintf(stderr, "Failed to invert matrix.\n");
        exit(EXIT_FAILURE);
    }
    return *inv;
}

template<typename Tresult, int N, typename T>
inline Tresult Mul(const SquareMatrix<N>& m, const T& v)
{
    Tresult result;
    for(int i = 0; i < N; ++i)
    {
        result[i] = 0;
        for(int j = 0; j < N; ++j)
        {
            result[i] += m[i][j] * v[j];
        }
    }
    return result;
}

template<int N, typename T>
inline T operator*(const SquareMatrix<N> &m, const T &v)
{
    return Mul<T>(m, v);
}

template<int N>
inline SquareMatrix<N> operator*(const SquareMatrix<N>& m1, const SquareMatrix<N>& m2)
{
    SquareMatrix<N> r;
    for(int i = 0; i < N; ++i)
    {
        for(int j = 0; j < N; ++j)
        {
            r[i][j] = 0;
            for(int k = 0; k < N; ++k)
            {
                r[i][j] = FMA(m1[i][k], m2[k][j], r[i][j]);
            }
        }
    }
    return r;
}

template<>
inline SquareMatrix<4> operator *(const SquareMatrix<4> &m1, 
                                  const SquareMatrix<4> &m2)
{
    SquareMatrix<4> r;
    for(int i = 0; i < 4; ++i)
    {
        for(int j = 0; j < 4; ++j)
        {
            r[i][j] = InnerProduct(m1[i][0], m2[0][j], m1[i][1], m2[1][j], m1[i][2],
                                   m2[2][j], m1[i][3], m2[3][j]);
        }
    }
    return r;
}

template<>
inline SquareMatrix<3> operator*(const SquareMatrix<3>& m1, const SquareMatrix<3>& m2)
{
    SquareMatrix<3> r;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            r[i][j] =
                InnerProduct(m1[i][0], m2[0][j], m1[i][1], m2[1][j], m1[i][2], m2[2][j]);
    return r;
}

template<typename C>
inline constexpr float EvaluatePolynomial(float t, C c)
{
    return c;
}

template<typename C, typename... Args>
inline constexpr float EvaluatePolynomial(float t, C c, Args... cRemaining)
{
    return FMA(t, EvaluatePolynomial(t, cRemaining...), c);
}

#endif