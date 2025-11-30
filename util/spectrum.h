#ifndef SPECTRUM
#define SPECTRUM
#include "taggedptr.h"
#include <math.h>
#include "vecmath.h"
#include "colorspace.h"
constexpr float Lambda_min = 360, Lambda_max = 830;
static constexpr int NSpectrumSamples = 4;
static constexpr float CIE_Y_integral = 106.856895;
inline float BlackBody(float lambda, float T)
{
    if(T <= 0)
    {
        return 0.0f;
    }
    const float c = 299792458.f;
    const float h = 6.62606957e-34f;
    const float kb = 1.3806488e-23f;
    float l = lambda * 1e-9f;
    float le = 2 * h * c * c / (Pow<5>(l) * (exp((h * c )/ (l * kb * T)) - 1));
    return le;
}
class BlackBodySpectrum;
class ConstantSpectrum;
class PiecewiseLinearSpectrum;
class DenselySampledSpectrum;
class RGBAlbedoSpectrum;
class RGBUnboundedSpectrum;
class RGBIlluminantSpectrum;

class Spectrum : public TaggedPointer<ConstantSpectrum, DenselySampledSpectrum,
                                      PiecewiseLinearSpectrum, RGBAlbedoSpectrum,
                                      RGBUnboundedSpectrum, RGBIlluminantSpectrum,
                                      BlackBodySpectrum>
{
public:
    using TaggedPointer::TaggedPointer;
    
    std::string ToString() const;

    float operator()(float lambda) const;
    
    float MaxValue() const;
};

class SampledSpectrum
{
public:
    SampledSpectrum() = default;
    explicit SampledSpectrum(const float c) { values.fill(c); }

    SampledSpectrum(const std::span<const float> v)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] = v[i];
        }
    }
    
    SampledSpectrum operator+(const SampledSpectrum& s) const
    {
        SampledSpectrum ret = *this;
        ret += s;
        return ret;
    }

    SampledSpectrum& operator+=(const SampledSpectrum& s)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] += s.values[i];
        }
        return *this;
    }

    SampledSpectrum operator-(const SampledSpectrum& s) const
    {
        SampledSpectrum ret = *this;
        ret -= s;
        return ret;
    }

    SampledSpectrum& operator-=(const SampledSpectrum& s)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] -= s.values[i];
        }
        return *this;
    }

    SampledSpectrum operator*(const SampledSpectrum& s) const
    {
        SampledSpectrum ret = *this;
        ret *= s;
        return ret;
    }

    SampledSpectrum& operator*=(const SampledSpectrum& s)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] *= s.values[i];
        }
        return *this;
    }

    SampledSpectrum operator*(float a) const 
    {
        SampledSpectrum ret = *this;
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            ret.values[i] *= a;
        }
        return ret;
    }

    SampledSpectrum& operator*=(float a)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] *= a;
        }
        return *this;
    }

    friend SampledSpectrum operator*(float a, const SampledSpectrum& s)
    {
        return s * a;
    }

    SampledSpectrum& operator /=(const SampledSpectrum& s)
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] /= s.values[i];
        }
        return *this;
    }

    SampledSpectrum operator/(const SampledSpectrum& s) const
    {
        SampledSpectrum ret = *this;
        ret /= s;
        return ret;
    }

    SampledSpectrum& operator/=(float a)
    {
        CHECK_NE(a, 0.f);
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] /= a;
        }
        return *this;
    }

    SampledSpectrum operator/(float a) const
    {
        SampledSpectrum ret = *this;
        ret /= a;
        return ret;
    }

    SampledSpectrum operator-() const
    {
        SampledSpectrum ret = *this;
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            ret.values[i] = -ret.values[i];
        }
        return ret;
    }

    bool operator==(const SampledSpectrum& s) const
    {
        return values == s.values;
    }

    bool operator!=(const SampledSpectrum& s) const
    {
        return values != s.values;
    }

    std::string ToString() const;

    XYZ ToXYZ(const SampledWavelengths& lambda) const;
    RGB ToRGB(const SampledWavelengths& lambda, const RGBColorSpace& cs) const;

    SampledSpectrum(std::span<float> _values)
    {
        CHECK_EQ(_values.size(), NSpectrumSamples);
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            values[i] = _values[i];
        }
    }

    float operator[](int i) const
    {
        if(i < 0 || i >= NSpectrumSamples)
        {
            CHECK_EQ(1, 2);
        }
        return values[i];
    }

    float& operator[](int i)
    {
        if(i < 0 || i >= NSpectrumSamples)
        {
            CHECK_EQ(1, 2);
        }
        return values[i];
    }

    explicit operator bool() const
    {
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            if(values[i]) return true;
        }
        return false;
    }

    float MinComponentValue() const
    {
        float m = values[0];
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            m = std::min(m, values[i]);
        }
        return m;
    }

    float MaxComponentValue() const
    {
        float m = values[0];
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            m = std::max(m, values[i]);
        }
        return m;
    }

    float Average() const
    {
        float sum = 0.f;
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            sum += values[i];
        }
        return sum / NSpectrumSamples;
    }

private:
    std::array<float, NSpectrumSamples> values;
};

class ConstantSpectrum
{
public:
    ConstantSpectrum(float _c) : c(_c) {}
    float operator()(float lambda) const { return c; }
    float MaxValue() const { return c; }
    std::string ToString() const;
private:
    float c;
};

class DenselySampledSpectrum
{
public:
    DenselySampledSpectrum(Spectrum spec, int _lambda_min = Lambda_min, int _lambda_max = Lambda_max)
    : lambda_min(_lambda_min), lambda_max(_lambda_max), values(_lambda_max - _lambda_min + 1) 
    {
        if(spec)
        {
            for(int lambda = lambda_min; lambda <= lambda_max; ++lambda)
            {
                values[lambda - lambda_min] = spec(lambda);
            }
        }
    }

    SampledSpectrum Sample(const SampledWavelengths& lambda) const
    {
        SampledSpectrum s;
        for(int i = 0; i < NSpectrumSamples; ++i)
        {
            int offset = std::lround(lambda[i]) - lambda_min;
            if(offset < 0 || offset >= values.size())
            {
                s[i] = 0;
            }
            else
            {
                s[i] = values[offset];
            }
        }
        return s;
    }

    float operator()(float lambda) const
    {
        int offset = std::lround(lambda) - lambda_min;
        if(offset < 0 || offset >= values.size()) return 0.f;
        return values[offset];
    }

    float MaxValue() const 
    {
        return *std::max_element(values.begin(), values.end());
    }

private:
    int lambda_min, lambda_max;
    std::vector<float> values;
};

class PiecewiseLinearSpectrum
{
public:
    PiecewiseLinearSpectrum(
        std::span<const float> _lambdas,
        std::span<const float> _values);

    void Scale(float s)
    {
        for(auto&v : values)
        {
            v *= s;
        }
    }
    
    std::string ToString() const;

    float MaxValue() const;

    float operator()(float lambda) const;

    static PiecewiseLinearSpectrum* FromInterleaved(std::span<const float> samples, 
                                                    bool normalize);
private:
    std::vector<float> lambdas, values;
};

class BlackBodySpectrum
{
public:
    BlackBodySpectrum(float _T)
      : T(_T)
    {
        float lambdaMax = 2.8977721e-3f / T;
        normalizationFactor = 1.0f / BlackBody(lambdaMax * 1e9f, T);
    }
    float operator()(float lambda) const 
    {
        return normalizationFactor * BlackBody(lambda, T);
    }

    float MaxValue() const { return 1.f; }

    std::string ToString() const;
private:
    float T;
    float normalizationFactor;
};

class RGBAlbedoSpectrum
{
public:
    RGBAlbedoSpectrum(const RGBColorSpace& cs, RGB rgb);
    float operator()(float lambda) const { return rsp(lambda); }
    float MaxValue() const { return rsp.MaxValue(); }
private:
    RGBSigmoidPolynomial rsp;
};

class RGBUnboundedSpectrum
{
public:
    RGBUnboundedSpectrum(const RGBColorSpace& cs, RGB rgb);

    float operator()(float lambda) const { return rsp(lambda) * scale; }
    float MaxValue() const { return rsp.MaxValue() * scale; }
private:
    float scale = 1.f;
    RGBSigmoidPolynomial rsp;
};

class RGBIlluminantSpectrum
{
public:
    RGBIlluminantSpectrum(const RGBColorSpace& cs, RGB rgb);

    float operator()(float lambda) const 
    { 
        if(!illuminant) return 0.f;
        return scale * rsp(lambda) * (*illuminant)(lambda);
    }

    float MaxValue() const
    {
        if(!illuminant) return 0.f;
        return scale * rsp.MaxValue() * illuminant->MaxValue();
    }
private:
    float scale;
    RGBSigmoidPolynomial rsp;
    const DenselySampledSpectrum *illuminant;
};

inline SampledSpectrum SafeDiv(const SampledSpectrum& a, const SampledSpectrum& b)
{
    SampledSpectrum r;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        r[i] = b[i] ? a[i] / b[i] : 0.f;
    }
    return r;
}

template<typename U, typename V>
inline SampledSpectrum Clamp(const SampledSpectrum& a, U low, V high)
{
    SampledSpectrum r;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        r[i] = std::clamp(a[i], low, high);
    }
    return r;
}

inline SampledSpectrum ClampZero(const SampledSpectrum& a)
{
    SampledSpectrum ret;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        ret[i] = std::max<float>(0.f, a[i]);
    }
    return ret;
}

inline SampledSpectrum Sqrt(const SampledSpectrum& s)
{
    SampledSpectrum ret;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        ret[i] = std::sqrt(s[i]);
    }
    return ret;
}

inline SampledSpectrum SafeSqrt(const SampledSpectrum& s)
{
    SampledSpectrum ret;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        ret[i] = std::sqrt(std::max<float>(0.0f, s[i]));
    }
    return ret;
}

inline SampledSpectrum Pow(const SampledSpectrum& s, float e)
{
    SampledSpectrum ret;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        ret[i] = std::pow(s[i], e);
    }
    return ret;
}

inline SampledSpectrum Exp(const SampledSpectrum& s)
{
    SampledSpectrum ret;
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        ret[i] = std::exp(s[i]);
    }
    return ret;
}

inline SampledSpectrum Lerp(float t, const SampledSpectrum& s1, 
                           const SampledSpectrum& s2)
{
    return (1 - t) * s1 + t * s2;
}

inline SampledSpectrum Bilerp(std::array<float, 2> p, std::span<const SampledSpectrum> v)
{
    return ((1 - p[0]) * (1 - p[1]) * v[0] + p[0] * (1 - p[1]) * v[1] +
            (1 - p[0]) * p[1] * v[2] + p[0] * p[1] * v[3]);
}

class SampledWavelengths
{
public:
    static SampledWavelengths SampleUniform(float u, float lambda_min = Lambda_min, float lambda_max = Lambda_max);

    bool operator==(const SampledWavelengths& other) const 
    {
        return lambda == other.lambda && pdf == other.pdf;
    }

    bool operator!=(const SampledWavelengths& other) const
    {
        return lambda != other.lambda || pdf != other.pdf;
    }

    std::string ToString() const;

    float operator[](int i) const { return lambda[i]; }

    float& operator[](int i) { return lambda[i]; }

    SampledSpectrum PDF() const { return SampledSpectrum(pdf); }

    void TerminateSecondary()
    {
        if(SecondaryTerminated()) return;

        for(int i = 1; i < NSpectrumSamples; ++i)
        {
            pdf[i] = 0;
        }
        pdf[0] /= NSpectrumSamples;
    }

    bool SecondaryTerminated() const 
    {
        for(int i = 1; i < NSpectrumSamples; ++i)
        {
            if(pdf[i] != 0) return false;
        }
        return true;
    }

private:
    std::array<float, NSpectrumSamples> lambda, pdf;
};

namespace Spectra
{
    inline const DenselySampledSpectrum& X()
    {
        extern DenselySampledSpectrum* x;
        return *x;
    }

    inline const DenselySampledSpectrum& Y()
    {
        extern DenselySampledSpectrum* y;
        return *y;
    }

    inline const DenselySampledSpectrum& Z()
    {
        extern DenselySampledSpectrum* z;
        return *z;
    }
}

inline float InnerProduct(Spectrum f, Spectrum g)
{
    float integral = 0.f;
    for(float lambda = Lambda_min; lambda <= Lambda_max; ++lambda)
    {
        integral += f(lambda) * g(lambda);
    }
    return integral;
}

XYZ SpectrumToXYZ(Spectrum s);

#endif