#ifndef CAMERA_H
#define CAMERA_H
#include "rtweekend.h"
#include "hittable.h"
#include "material.h"
#include "pdf.h"
#include "RTV.h"
#include "parallel.h"
#include <tuple>
#include <chrono>
class camera{
public:
    double aspect_ratio=16.0/9.0;
    int    image_width=400;
    int    samples_per_pixel=10;
    int    max_depth=10;
    double vfov = 90;
    Point3 lookfrom = Point3(0,0,0);
    Point3 lookat = Point3(0,0,-1);
    vec3 vup = vec3(0,1,0);
    double defocus_angle = 0;
    double focus_dist = 10;
    color background;
    void render(const hittable& world, const hittable& lights){
        initialize();
        std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";
        #define NO_VRS
        #ifdef USE_VRS

        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        for(int RTVStep = 1; RTVStep <= 3; RTVStep++)
        {
            for(int index = 0; index < rtvToPixel[RTVStep].size(); index++)
            {
                int i = rtvToPixel[RTVStep][index].first;
                int j = rtvToPixel[RTVStep][index].second;
                auto[shaderX, shaderY] = getCenterPixel(i, j, RTVStep);
                color pixel_color(0, 0, 0);
                for(int sampleu = 0; sampleu < sqrt_spp; sampleu++)
                {
                    for(int samplev = 0; samplev < sqrt_spp; samplev++)
                    {
                        Ray r = get_ray(i, j, sampleu, samplev);
                        pixel_color += ray_color(r,max_depth,world, lights);
                    }
                }
                pixel_color *= recip_sqrt_spp;
                for(int spiltY = 0; spiltY < RTVOFFSET::sizeX[RTVStep]; spiltY++)
                {
                    for(int spiltX = 0; spiltX < RTVOFFSET::sizeX[RTVStep]; spiltX++)
                    {
                        int shaderPixelX = i + spiltX;
                        int shaderPixelY = j + spiltY;
                        colorBuffer[shaderPixelY * image_width + shaderPixelX] = pixel_color;
                    }
                }
            }
        }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        std::cerr << "render time: " << time_span.count() << "ms\n";
        #endif
        
        #ifdef NO_VRS
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

        Bounds2i image(0.0f, 0.0f, image_width, image_height);
        
        ParallelFor2D(image, [&](Point2i p){
            color pixel_color(0,0,0);
            for(int sampleu = 0; sampleu < sqrt_spp; sampleu++)
            {
                for(int samplev = 0; samplev < sqrt_spp; samplev++)
                {
                    Ray r = get_ray(p.x, p.y, sampleu, samplev);
                    pixel_color += ray_color(r,max_depth, world, lights);
                }
            }
            pixel_color *= recip_sqrt_spp;
            colorBuffer[p.y * image_width + p.x] = pixel_color;
        });

        // for(int j = 0;j<image_height;j++){
        //     for(int i =0 ;i < image_width ;i ++ ){
        //         color pixel_color(0,0,0);
        //         for(int sampleu = 0; sampleu < sqrt_spp; sampleu++)
        //         {
        //             for(int samplev = 0; samplev < sqrt_spp; samplev++)
        //             {
        //                 Ray r = get_ray(i, j, sampleu, samplev);
        //                 pixel_color += ray_color(r,max_depth,world, lights);
        //             }
        //         }
        //         pixel_color *= recip_sqrt_spp;
        //         colorBuffer[j * image_width + i] = pixel_color;
        //     }
        // }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> time_span = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
        std::cerr << "render time: " << time_span.count() << "ms\n";
        #endif
        for(int j = 0;j<image_height;j++){
            for(int i =0 ;i < image_width ;i ++ ){
                write_color(std::cout, colorBuffer[j * image_width + i]);
            }
        }
        std::clog << "\rDone.                     \n";
    }
private:
    int image_height;
    Point3 center;
    Point3 pixel00_loc;
    vec3 pixel_delta_u;
    vec3 pixel_delta_v;
    double pixel_sample_scale;
    vec3 u,v,w;
    vec3 defocus_disk_u;
    vec3 defocus_disk_v;
    int sqrt_spp;
    double recip_sqrt_spp;
    std::vector<color> colorBuffer;
    std::vector<std::vector<std::pair<int, int>>> rtvToPixel;
    int* belongRTV;
    void initialize(){
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1 ) ? 1 : image_height;
        std::cerr << image_width << " " << image_height << "\n";
        center = lookfrom;
        pixel_sample_scale = 1.0 / samples_per_pixel;
        sqrt_spp = static_cast<int>(std::sqrt(samples_per_pixel));
        recip_sqrt_spp = 1.0 / (sqrt_spp * sqrt_spp);
        //viewpoer
        
        double theta = degrees_to_radians(vfov);
        double viewport_height= std::tan(theta/2) * focus_dist * 2;
        
        double viewport_width = viewport_height * static_cast<double>(image_width)/ static_cast<double>(image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup , w));
        v = unit_vector(cross(w,u));
        
        vec3 viewport_u = u*viewport_width;
        vec3 viewport_v= -v*viewport_height;
        
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        vec3 viewport_upper_left = center - focus_dist*w - viewport_u / 2 -viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u+pixel_delta_v);
        double defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle/2.0));
        defocus_disk_u = defocus_radius * u;
        defocus_disk_v = defocus_radius * v;
        
        colorBuffer.resize(image_width * image_height);
        rtvToPixel.resize(5);
        belongRTV = new int[image_width * image_height]{0};
        setMask(rtvToPixel, image_width, image_height, belongRTV);
        
    }
    color ray_color(const Ray& r,int depth,const hittable& world, const hittable& lights) const{
        if(depth <= 0 ){
            return color(0,0,0);
        }
        hit_record rec;
        if(!world.hit(r,interval(0.001,infinity),rec)){ return background; }
        
        color attenuation;
        Ray scattered;
        double pdf_value;
        color color_from_emission = rec.mat->emitted(r, rec, rec.u, rec.v, rec.p);
        if(!rec.mat->scatter(r,rec,attenuation,scattered, pdf_value)){
            return color_from_emission;
        }
        hittable_pdf light_pdf(lights, rec.p);
        scattered = Ray(rec.p, light_pdf.generate(), r.time());
        pdf_value = light_pdf.value(scattered.direction());
        double scatter_pdf = rec.mat->scattering_pdf(r, rec, scattered);
        color color_from_scatter = (scatter_pdf * attenuation * ray_color(scattered,depth-1,world, lights)) / pdf_value;
        return color_from_emission + color_from_scatter;
    }
    vec3 sample_square() const {
        return vec3(random_double()-0.5,random_double()-0.5,0.0);
    }

    vec3 sample_square_stratified(int s_i, int s_j) const{
        auto px = ((s_i + random_double()) * recip_sqrt_spp) - 0.5;
        auto py = ((s_j + random_double()) * recip_sqrt_spp) - 0.5;
        return vec3(px, py, 0.0); 
    }

    Ray get_ray(int i,int j, int s_i, int s_j) const {
        vec3 offset=sample_square_stratified(s_i, s_j);
        vec3 pixel_sample=pixel00_loc + 
                          ((i+ offset.x()) * pixel_delta_u)+
                          ((j+ offset.y()) * pixel_delta_v);
        vec3 ray_origin = (defocus_angle <= 0 ) ? center : defocus_disk_sample();
        vec3 direction = pixel_sample - ray_origin;
        direction = unit_vector(direction);
        double ray_time = random_double();
        return Ray(ray_origin,direction,ray_time);
    }
    Point3 defocus_disk_sample() const {
        vec3 p = random_in_unit_disk();
        return center + (p.x() * defocus_disk_u) + (p.y()* defocus_disk_v);
    }
    std::tuple<int, int> getCenterPixel(int leftPointX, int leftPointY, int rate)
    {
        int sizeXOffset = (RTVOFFSET::sizeX[rate] - 1) / 2;
        int sizeYOffset = (RTVOFFSET::sizeY[rate] - 1) / 2;
        return std::make_tuple(leftPointX + sizeXOffset, leftPointY + sizeYOffset);
    }
};
#endif