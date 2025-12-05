#ifndef COLORSPACE
#define COLORSPACE
#include "rtweekend.h"
#include "spectrum.h"
#include "vecmath.h"
#include "color.h"
class RGBColorSpace
{
public:
    RGBColorSpace(Point2f r, Point2f g, Point2f b, 
                  Spectrum illumiant, const RGBToSpectrumTable* rgbToSpectrumTable);

    RGBSigmoidPolynomial ToRGBCoeffs(RGB rgb) const;
    Point2f r, g, b, w;
    DenselySampledSpectrum illuminant;
    SquareMatrix<3> XYZFromRGB, RGBFromXYZ;
    static const RGBColorSpace *sRGB, *DCI_P3, *Rec2020, *ACES2065_1;

    RGB ToRGB(const XYZ& xyz) const { return Mul<RGB>(RGBFromXYZ, xyz); }

    XYZ ToXYZ(const RGB& rgb) const { return Mul<XYZ>(XYZFromRGB, rgb); }

    bool operator == (const RGBColorSpace& cs) const 
    {
        return (r == cs.r && g == cs.g && b == cs.b && w == cs.w &&
        rgbToSpectrumTable == cs.rgbToSpectrumTable);
    }
    bool operator != (const RGBColorSpace& cs) const 
    {
        return (r != cs.r || g != cs.g || b != cs.b || w != cs.w ||
        rgbToSpectrumTable != cs.rgbToSpectrumTable);
    }

    std::string ToString() const;

    static const RGBColorSpace *GetName(std::string n);
    static const RGBColorSpace *Lookup(Point2f r, Point2f g, Point2f b, Point2f w);
private:
    const RGBToSpectrumTable *rgbToSpectrumTable;
};

SquareMatrix<3> ConvertRGBColorSpace(const RGBColorSpace& from, const RGBColorSpace& to);
#endif