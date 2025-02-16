#ifndef MATERIAL_H
#define MATERIAL_H
#include "rtweekend.h"
#include "texture.h"
class hit_record;
class material{
public:
    virtual ~material() = default;
    virtual color emitted(double u,double v,const Point3& p) const{
        return color(0,0,0);
    }
    virtual bool scatter(
        const Ray& r_in,const hit_record& rec,color& attenuation,Ray& scattered
    ) const {
        return false;
    }
    virtual double scattering_pdf(const Ray& r_in, const hit_record& rec, const Ray& scattered)
        const
    {
        return 0;
    }
};
class lambertian : public material{
public:
    lambertian(const color& albedo) :tex(make_shared<solid_color>(albedo)){}
    lambertian(shared_ptr<texture> tex) : tex(tex){}
    bool scatter(
        const Ray& r_in,const hit_record& rec,color& attenuation,Ray& scattered
    ) const override{
        vec3 direction = random_on_hemisphere(rec.normal);
        if(direction.near_zero()) direction = rec.normal;

        direction = unit_vector(direction);
        scattered = Ray(rec.p , direction, r_in.time());
        attenuation = tex->value(rec.u,rec.v,rec.p);
        return true;
    }
    double scattering_pdf(const Ray& r_in, const hit_record& rec, const Ray& scattered) 
        const override
        {
            auto cosine = dot(rec.normal, unit_vector(scattered.direction()));
            return cosine < 0 ? 0 : cosine / pi;
        }
private:
    color albedo;
    shared_ptr<texture> tex;
};
class metal : public material{
public:
    metal(const color& albedo,const double& fuzz) : albedo(albedo),fuzz(fuzz < 1? fuzz:1){}
    bool scatter(const Ray& r_in,const hit_record& rec, color& attenuation,Ray& scattered) const override{
        vec3 reflected = reflect(r_in.direction(),rec.normal);
        vec3 fuzz_direction=fuzz* random_unit_vector();
        reflected = reflected + fuzz_direction;
        reflected = unit_vector(reflected);
        scattered = Ray(rec.p,reflected,r_in.time());
        attenuation = albedo;
        return dot(rec.normal,scattered.direction()) > 0;
    }
private:
    color albedo;
    double fuzz;
};
class dielectric : public material{
public:
    dielectric(double refraction_index) : refraction_index(refraction_index){}
    bool scatter (
        const Ray& r_in, const hit_record& rec , color& attenuation , Ray& scattered
    ) const override{
        attenuation = color(1.0,1.0,1.0);
        double ri = rec.front_face? 1.0 / refraction_index:refraction_index ;
        double cos_theta = -dot(r_in.direction(),rec.normal);
        double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);
        vec3 direction;
        bool connot_refract = ri*sin_theta > 1.0;
        if(connot_refract||reflectance(cos_theta,ri) > random_double()){
            direction=reflect(r_in.direction(),rec.normal);
        }
        else{
            direction = refract(r_in.direction(),rec.normal,ri);
        }
        scattered = Ray(rec.p,direction,r_in.time());
        return true;
    }
private:    
    double refraction_index; 
    static double reflectance(double cosine,double refraction_index){
        double r0 = (1-refraction_index) / (1+refraction_index);
        r0=r0*r0;
        return r0+(1-r0)*std::pow(1-cosine,5);
    }
};
class diffuse_light : public material{
public:
    diffuse_light(shared_ptr<texture> tex) : tex(tex){}
    diffuse_light(const color& emit) : tex(make_shared<solid_color>(emit)) {}
    color emitted(double u,double v,const Point3& p) const override{
        return tex->value(u,v,p);
    }
private:
    shared_ptr<texture> tex;
};
class isotropic : public material{
public:
    isotropic(const color& albedo) : tex(make_shared<solid_color>(albedo)) {}
    isotropic(shared_ptr<texture> tex) : tex(tex) {}
    virtual bool scatter(
        const Ray& r_in,const hit_record& rec,color& attenuation,Ray& scattered
    ) const  override{
        scattered = Ray(rec.p,random_unit_vector(),r_in.time());
        attenuation = tex -> value(rec.u,rec.v,rec.p);
        return true;
    }
private:
    shared_ptr<texture> tex;
};
#endif