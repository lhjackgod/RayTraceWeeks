#include "colorspace.h"

const RGBColorSpace* RGBColorSpace::sRGB;
const RGBColorSpace* RGBColorSpace::DCI_P3;
const RGBColorSpace* RGBColorSpace::Rec2020;
const RGBColorSpace* RGBColorSpace::ACES2065_1;

RGBColorSpace::RGBColorSpace(
    Point2f r, Point2f g, Point2f b,
    Spectrum illuminant, const RGBToSpectrumTable* rgbToSpectrumTable
)
 : r(r), g(g), b(b), rgbToSpectrumTable(rgbToSpectrumTable)
{
    XYZ W = SpectrumToXYZ(illuminant); //illuminant XYZ
    w = W.xy();
    XYZ R = XYZ::FromxyY(r), G = XYZ::FromxyY(g), B = XYZ::FromxyY(b);

    SquareMatrix<3> rgb(R.X, G.X, B.X,
                        R.Y, G.Y, B.Y,
                        R.Z, G.Z, B.Z);
    XYZ C = InvertOrExit(rgb) * W;
    XYZFromRGB = rgb * SquareMatrix<3>::Diag(C[0], C[1], C[2]);
    RGBFromXYZ = InvertOrExit(XYZFromRGB);
}

SquareMatrix<3> ConvertRGBColorSpace(const RGBColorSpace& from, const RGBColorSpace& to)
{
    if(from == to) return {};
    return to.XYZFromRGB * from.RGBFromXYZ;
}

const RGBColorSpace* RGBColorSpace::GetName(std::string n)
{
    std::string name;
    std::transform(n.begin(), n.end(), std::back_inserter(name), ::tolower);
    if (name == "aces2065-1")
        return ACES2065_1;
    else if (name == "rec2020")
        return Rec2020;
    else if (name == "dci-p3")
        return DCI_P3;
    else if (name == "srgb")
        return sRGB;
    else
        return nullptr;
}

const RGBColorSpace* RGBColorSpace::Lookup(Point2f r, Point2f g, Point2f b, Point2f w)
{
    auto closeEnough = [](const Point2f &a, const Point2f &b) {
        return ((a.x == b.x || std::abs((a.x - b.x) / b.x) < 1e-3) &&
                (a.y == b.y || std::abs((a.y - b.y) / b.y) < 1e-3));
    };
    for(const RGBColorSpace *cs : {sRGB, DCI_P3, Rec2020, ACES2065_1})
    {
        if(closeEnough(cs->r, r) && closeEnough(cs->g, g) && closeEnough(cs->b, b) && closeEnough(cs->w, w))
            return cs;
    }
    return nullptr;
}

RGBSigmoidPolynomial RGBColorSpace::ToRGBCoeffs(RGB rgb) const
{
    return (*rgbToSpectrumTable)(rgb);
}

std::string RGBColorSpace::ToString() const
{
    return std::format("[ RGBColorSpace r: {} g: {} b: {} w: {} illuminant: {} RGBToXYZ: {} XYZToRGB: {} ]", r.ToString(), g.ToString(), b.ToString(), w.ToString(), illuminant.ToString(), XYZFromRGB.ToString(), RGBFromXYZ.ToString());
}