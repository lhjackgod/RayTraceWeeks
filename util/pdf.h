#ifndef PDF_H
#define PDF_H
#include "vec3.h"
#include "ONB.h"
class pdf
{
public:
    virtual ~pdf(){}
    virtual double value(const vec3& direction) const = 0;
    virtual vec3 generate() const = 0;
};

class shpere_pdf : public pdf
{
public:
    shpere_pdf(){}
    double value(const vec3& direction) const override{
        return 1.0 / (4 * pi);
    }
    vec3 generate() const override{
        return random_unit_vector();
    }
};

class cosine_pdf : public pdf
{
public:
    cosine_pdf(const vec3& w) : uvw(w){}
    double value(const vec3& direction) const override{
        auto cosine = dot(unit_vector(direction), uvw.w());
        return std::max(0.0, cosine / pi);
    }
    vec3 generate() const override
    {
        return uvw.transform(random_cosine_direction());
    }
private:
    ONB uvw;
};

#endif