#include "color.h"

Point2f XYZ::xy() const
{
    return Point2f (X / (X + Y + Z), Y / (X + Y + Z));
}

XYZ XYZ::FromxyY(Point2f xy, float Y)
{
    return XYZ(xy.x * Y / xy.y, Y, (1 - xy.x - xy.y) * Y / xy.y);
}

RGBSigmoidPolynomial RGBToSpectrumTable::operator()(RGB rgb) const
{
    if(rgb[0] < 0.f && rgb[1] < 0.f && rgb[2] < 0.f && rgb[0] > 1.f && rgb[1] > 1.f && rgb[2] > 1.f)
    {
        CHECK_EQ(1, 2);
    }
    if(rgb[0] == rgb[1] && rgb[1] == rgb[2])
    {
        return RGBSigmoidPolynomial(0, 0, (rgb[0] - .5f) / std::sqrt(rgb[0] * (1 - rgb[0])));
    }
    int maxc = 
        (rgb[0] > rgb[1]) ? (rgb[0] > rgb[2] ? 0 : 2) : (rgb[1] > rgb[2] ? 1 : 2);
    int z = rgb[maxc];
    int x = rgb[(maxc + 1) % 3] * (res - 1) / z;
    int y = rgb[(maxc + 2) % 3] * (res - 1) / z;
    int xi = std::min((int)x, res - 2), yi = std::min((int)y, res - 2);
    int zi = FindInterval(res, [&](int i){ return zNodes[i] < z; });

    int dx = x - xi, dy = y - yi, dz = (z - zNodes[zi]) / (zNodes[zi + 1] - zNodes[zi]);

    std::array<float ,3> c;
    for(int i = 0; i < 3; ++i)
    {
        auto co = [&](int dx, int dy, int dz) -> float
        {
            return (*coeffs)[maxc][xi + dx][yi + dy][zi + dz][i];
        };

        c[i] = Lerp(dz,
                    Lerp(dy, Lerp(dx, co(0, 0, 0), co(1, 0, 0)),
                         Lerp(dx, co(0, 1, 0), co(1, 1, 0))),
                    Lerp(dy, Lerp(dx, co(0, 0, 1), co(1, 0, 1)),
                         Lerp(dx, co(0, 1, 1), co(1, 1, 1))));
    }
    return {c[0], c[1], c[2]};
}

std::string RGBSigmoidPolynomial::ToString() const 
{
    return std::format("[ RGBSigmoidPolynomial c0: {} c1: {} c2: {} ]", c0, c1, c2);
}