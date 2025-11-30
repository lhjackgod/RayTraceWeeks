#ifndef COLOR_H
#define COLOR_H
#include "rtweekend.h"
using color=vec3;

inline double linear_to_gamma(double linear_component){
    if(linear_component > 0){
        return std::sqrt(linear_component);
    }
    return 0;
}
inline void write_color(std::ostream& out, const color& pixel_color){
    auto r=pixel_color.x();
    auto g=pixel_color.y();
    auto b=pixel_color.z();
    r=linear_to_gamma(r);
    g=linear_to_gamma(g);
    b=linear_to_gamma(b);
    static const interval intensity(0.000,0.999);
    int rbyte = int(256*intensity.clamp(r));
    int gbyte = int(256*intensity.clamp(g));
    int bbyte = int(256*intensity.clamp(b));
    //actural_color=color(rbyte,gbyte,bbyte);
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}


class XYZ
{
public:
    XYZ() = default;
    XYZ(float x, float y, float z) : X(x), Y(y), Z(z) {}

    XYZ operator / (float f) const { return {X / f, Y / f, Z / f}; }

    float Average() const { return (X + Y + Z) / 3.f; }

    Point2f xy() const;

    static XYZ FromxyY(Point2f xy, float y = 1.f );

    XYZ &operator+=(const XYZ& s)
    {
        X += s.X;
        Y += s.Y;
        Z += s.Z;
        return *this;
    }

    XYZ operator + (const XYZ& s) const 
    {
        XYZ ret = *this;
        return ret += s;
    }

    XYZ& operator -= (const XYZ& s)
    {
        X -= s.X;
        Y -= s.Y;
        Z -= s.Z;
        return *this;
    }

    XYZ operator - (const XYZ& s) const 
    {
        XYZ ret = *this;
        return ret -= s;
    }

    friend XYZ operator - (float a, XYZ s) { return {a - s.X, a - s.Y, a - s.Z}; }

    XYZ& operator *= (const XYZ& s)
    {
        X *= s.X;
        Y *= s.Y;
        Z *= s.Z;
        return *this;
    }

    XYZ operator * (const XYZ& s) const 
    {
        XYZ ret = *this;
        return ret *= s;
    }

    XYZ operator * (float a) const 
    {
        return {a * X, a * Y, a * Z};
    }

    XYZ& operator *= (float a)
    {
        X *= a;
        Y *= a;
        Z *= a;
        return *this;
    }

    XYZ& operator /= (const XYZ& s)
    {
        X /= s.X;
        Y /= s.Y;
        Z /= s.Z;
        return *this;
    }

    XYZ operator / (const XYZ& s) const 
    {
        XYZ ret = *this;
        return ret /= s;
    }

    XYZ &operator /= (float a)
    {
        CHECK_NE(a, 0.f);
        X /= a;
        Y /= a;
        Z /= a;
        return *this;
    }

    XYZ operator / (float a) const 
    {
        XYZ ret = *this;
        return ret /= a;
    }

    XYZ operator -() const 
    {
        return { -X, -Y, -Z };
    }

    bool operator == (const XYZ &s) const { return X == s.X && Y == s.Y && Z == s.Z; }

    bool operator != (const XYZ& s) const { return X != s.X || Y != s.Y || Z != s.Z; }

    float operator[](int i) const 
    {
        CHECK_GE(i, 0);
        CHECK_LT(i, 3);
        return i == 0 ? X : (i == 1 ? Y : Z);
    }

    float& operator[](int i)
    {
        CHECK_GE(i, 0);
        CHECK_LT(i, 3);
        return i == 0 ? X : (i == 1 ? Y : Z);
    }

    std::string ToString() const ;

    float X = 0.f, Y = 0.f, Z = 0.f;
};

class RGB
{
public:
    RGB() = default;
    RGB(float r, float g, float b) : r(r), g(g), b(b) {}

    RGB& operator += (const RGB& other)
    {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;
    }

    RGB operator +(const RGB& other) const
    {
        return {r + other.r, g + other.g, b + other.b};
    }

    RGB& operator -=(RGB s)
    {
        r -= s.r;
        g -= s.g;
        b -= s.b;
        return *this;
    }

    RGB operator -(RGB s) const
    {
        return {r - s.r, g - s.g, b - s.b};
    }

    friend RGB operator-(float a, RGB s) { return {a - s.r, a - s.g, a - s.b}; }

    RGB& operator *=(RGB s)
    {
        r *= s.r;
        g *= s.g;
        b *= s.b;
        return *this;
    }

    RGB operator *(RGB s) const 
    {
        return {r * s.r, g * s.g, b * s.b};
    }

    RGB operator *(float a) const 
    {
        return {r * a, g * a, b * a};
    }

    RGB& operator *=(float a) 
    {
        r *= a;
        g *= a;
        b *= a;
        return *this;
    }

    friend RGB operator *(float a, RGB s) { return {a * s.r, a * s.g, a * s.b}; }

    RGB &operator/=(RGB s)
    {
        r /= s.r;
        g /= s.g;
        b /= s.b;
        return *this;
    }

    RGB operator /(RGB s) const 
    {
        return {r / s.r, g / s.g, b / s.b};
    }

    RGB& operator /=(float a)
    {
        r /= a;
        g /= a;
        b /= a;
        return *this;
    }

    RGB operator /(float a) const 
    {
        return {r / a, g / a, b / a};
    }

    RGB operator -() const { return {-r, -g, -b}; }

    float Average() const { return (r + g + b) / 3; }

    bool operator ==(const RGB& s) const { return r == s.r && g == s.g && b == s.b; }

    bool operator !=(const RGB& s) const { return r != s.r || g != s.g || b != s.b; }

    float operator[](int c) const 
    {
        CHECK_GE(c, 0);
        CHECK_LT(c, 3);
        return (c == 0) ? r : (c == 1) ? g : b;
    }

    float& operator[](int c)
    {
        CHECK_GE(c, 0);
        CHECK_LT(c, 3);
        return (c == 0) ? r : (c == 1) ? g : b;
    }

    std::string ToString() const;

    float r = 0.f, g = 0.f, b = 0.f;
};

class RGBSigmoidPolynomial
{
public:
    RGBSigmoidPolynomial() = default;
    RGBSigmoidPolynomial(float c0, float c1, float c2) 
     : c0(c0), c1(c1), c2(c2) {}
    
    std::string ToString() const;

    float operator()(float lambda) const
    {
        return s(EvaluatePolynomial(lambda, c0, c1, c2));
    }

    float MaxValue() const
    {
        float result = std::max((*this)(360), (*this)(830));
        float lambda = -c1 / (2.0f * c0);
        if(lambda >= 360.f && lambda <= 830.f)
        {
            result = std::max(result, (*this)(lambda));
        }
        return result;
    }

private:
    static float s(float x)
    {
        if(std::isinf(x))
        {
            return x > 0 ? 1.0f : 0.f;
        }
        return 0.5f + x / (2.0f * std::sqrt(1 + x * x));
    }
    float c0, c1, c2;
};

class RGBToSpectrumTable
{
public:
    static constexpr int res = 64;
    using CoefficientArray = float[3][res][res][res][3];
    RGBSigmoidPolynomial operator()(RGB rgb) const;
private:
    const float* zNodes;
    const CoefficientArray *coeffs;
};

#endif