#include "spectrum.h"


namespace Spectra
{
    namespace
    {
        std::map<std::string, Spectrum> namedSpectra;
    }
}

std::string Spectrum::ToString() const 
{
    
}

float Spectrum::operator()(float lambda) const 
{
    auto op = [&](auto ptr) { return (*ptr)(lambda); };
    return Dispatch(op);
}

float Spectrum::MaxValue() const 
{
    auto op = [](auto ptr){ return ptr->MaxValue(); };
    return Dispatch(op);
}

PiecewiseLinearSpectrum::PiecewiseLinearSpectrum(
    std::span<const float> _lambdas,
    std::span<const float> _values
)
 : lambdas(_lambdas.begin(), _lambdas.end()), values(_values.begin(), _values.end())
{
    CHECK_EQ(lambdas.size(), values.size());
    for(size_t i = 0; i < lambdas.size() - 1; ++i)
    {
        CHECK_LT(lambdas[i], lambdas[i + 1]);
    }
}

float PiecewiseLinearSpectrum::operator()(float lambda) const
{
    if(lambdas.empty() || lambda < lambdas.front() || lambda > lambdas.back()) 
        return 0.f;
    int o = FindInterval(lambdas.size(), [&](size_t i){ return lambdas[i] <= lambda; });
    if(lambdas[o] <= lambda && lambda <= lambdas[o + 1])
    {
        float t = (lambda - lambdas[o]) / (lambdas[o + 1] - lambdas[o]);
        return Lerp(t, values[o], values[o + 1]);
    }
    return 0.f;
}

float PiecewiseLinearSpectrum::MaxValue() const
{
    if(values.empty()) return 0.0f;
    return *std::max_element(values.begin(), values.end());
}

PiecewiseLinearSpectrum* PiecewiseLinearSpectrum::FromInterleaved(
    std::span<const float> samples, bool normalize
)
{
    int n = samples.size() / 2;
    std::vector<float> lambda, v;
    // Extend samples to cover range of visible wavelengths if needed.
    if(samples[0] > Lambda_min)
    {
        lambda.emplace_back(Lambda_min - 1);
        v.emplace_back(samples[1]);
    }
    for(size_t i = 0; i < n; ++i)
    {
        lambda.emplace_back(samples[2 * i]);
        v.emplace_back(samples[2 * i + 1]);
        if(i > 0)
        {
            CHECK_GT(lambda.back(), lambda[lambda.size() - 2]);
        }
    }
    if(lambda.back() < Lambda_max)
    {
        lambda.emplace_back(Lambda_max + 1);
        v.emplace_back(v.back());
    }
    PiecewiseLinearSpectrum* spec = new PiecewiseLinearSpectrum(
        lambda, v
    );
    if(normalize)
    {
        spec->Scale(CIE_Y_integral / InnerProduct(spec, &Spectra::Y()));
    }
    return spec;
}

XYZ SampledSpectrum::ToXYZ(const SampledWavelengths& lambda) const
{
    SampledSpectrum X = Spectra::X().Sample(lambda);
    SampledSpectrum Y = Spectra::Y().Sample(lambda);
    SampledSpectrum Z = Spectra::Z().Sample(lambda);

    SampledSpectrum pdf = lambda.PDF();

    return XYZ(
        SafeDiv(X * *this, pdf).Average(),
        SafeDiv(Y * *this, pdf).Average(),
        SafeDiv(Z * *this, pdf).Average()
    ) / CIE_Y_integral;

}

RGBAlbedoSpectrum::RGBAlbedoSpectrum(const RGBColorSpace& cs, RGB rgb)
{
    CHECK_LE(std::max({rgb.r, rgb.g, rgb.b}), 1.f);
    CHECK_GE(std::max({rgb.r, rgb.g, rgb.b}), 0.f);
    rsp = cs.ToRGBCoeffs(rgb);
}

RGBUnboundedSpectrum::RGBUnboundedSpectrum(const RGBColorSpace& cs, RGB rgb)
{
    float m = std::max({rgb.r, rgb.g, rgb.b});
    scale = 2.f * m;
    rsp = cs.ToRGBCoeffs(scale ? rgb / scale : RGB(0.0f, 0.0f, 0.0f));
}

RGBIlluminantSpectrum::RGBIlluminantSpectrum(const RGBColorSpace& cs, RGB rgb)
 : illuminant(&cs.illuminant)
{
    float m = std::max({rgb.r, rgb.g, rgb.b});
    scale = 2.f * m;
    rsp = cs.ToRGBCoeffs(scale ? rgb / scale : RGB(0, 0, 0));
}

RGB SampledSpectrum::ToRGB(const SampledWavelengths& lambda, const RGBColorSpace& cs) const
{
    XYZ xyz = ToXYZ(lambda);
    return cs.ToRGB(xyz);
}

SampledWavelengths SampledWavelengths::SampleUniform(float u, float lambda_min, float lambda_max)
{
    SampledWavelengths swl;
    swl.lambda[0] = Lerp(u, lambda_min, lambda_max);
    // Initialize the rest of the wavelengths.
    float delta = (lambda_max - lambda_min) / NSpectrumSamples;
    for(int i = 1; i < NSpectrumSamples; ++i)
    {
        swl.lambda[i] = swl.lambda[i - 1] + delta;
        if(swl.lambda[i] > lambda_max)
        {
            swl.lambda[i] = lambda_min + (swl.lambda[i] - lambda_max);
        }
    }
    for(int i = 0; i < NSpectrumSamples; ++i)
    {
        swl.pdf[i] = 1 / (lambda_max - lambda_min);
    }
    return swl;
}

XYZ SpectrumToXYZ(Spectrum s)
{
    return XYZ(InnerProduct(&Spectra::X(), s), InnerProduct(&Spectra::Y(), s), InnerProduct(&Spectra::Z(), s)) / CIE_Y_integral;
}