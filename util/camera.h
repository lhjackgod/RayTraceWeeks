#ifndef CAMERA_H
#define CAMERA_H
#include "rtweekend.h"
#include "hittable.h"
#include <opencv2/opencv.hpp>
#include "material.h"
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
    void render(const hittable& world){
        initialize();
        cv::Mat image(image_height,image_width,CV_8UC3);
        for(int j = 0;j<image_height;j++){
            for(int i =0 ;i < image_width ;i ++ ){
                color pixel_color(0,0,0);
                for(int sample=0;sample<samples_per_pixel;sample++){
                    Ray r = get_ray(i,j);
                    pixel_color += ray_color(r,max_depth,world);
                }
                pixel_color *= pixel_sample_scale;
                color preColor;
                write_color(std::cout,pixel_color,preColor);
                cv::Vec3b pre_color(
                    (uchar)(preColor.z()),
                    (uchar)(preColor.y()),
                    (uchar)(preColor.x())
                );
                image.at<cv::Vec3b>(j,i) = pre_color;
            }
        }
        cv::imshow("jack",image);
        cv::imwrite("jack.png",image);
        cv::waitKey(0);
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
    void initialize(){
        image_height = static_cast<int>(image_width / aspect_ratio);
        image_height = (image_height < 1 ) ? 1 : image_height;
        center = lookfrom;
        pixel_sample_scale = 1.0 / samples_per_pixel;
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

    }
    color ray_color(const Ray& r,int depth,const hittable& world) const{
        if(depth <= 0 ){
            return color(0,0,0);
        }
        hit_record rec;
        if(!world.hit(r,interval(0.001,infinity),rec)){ return background; }
        
        color attenuation;
        Ray scattered;
        color color_from_emission = rec.mat->emitted(rec.u,rec.v,rec.p);
        if(!rec.mat->scatter(r,rec,attenuation,scattered)){
            return color_from_emission;
        }
        color color_from_scatter = attenuation * ray_color(scattered,depth-1,world);
        return color_from_emission + color_from_scatter;
        // if(world.hit(r,interval(0.001,infinity),rec)){
        //     color attenuation;
        //     Ray scattered;
        //     if(rec.mat->scatter(r,rec,attenuation,scattered)){
        //         return attenuation*(ray_color(scattered,depth-1,world));
        //     }
        //     return color(0,0,0);
        // }
        // double a=0.5*(1.0+r.direction().y());
        // return (1.0-a)*color(1.0,1.0,1.0)+a*color(0.5,0.7,1.0);
    }
    vec3 sample_square() const {
        return vec3(random_double()-0.5,random_double()-0.5,0.0);
    }
    Ray get_ray(int i,int j) const {
        vec3 offset=sample_square();
        vec3 pixel_sample=pixel00_loc + 
                          ((i+offset.x()) * pixel_delta_u)+
                          ((j+offset.y()) * pixel_delta_v);
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
};
#endif