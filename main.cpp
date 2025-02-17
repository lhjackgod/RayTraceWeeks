#include "util/rtweekend.h"
#include "util/sphere.h"
#include "util/camera.h"
#include "util/hittable.h"
#include "util/hittable_list.h"
#include "util/bvh.h"
#include "util/texture.h"
#include "util/quad.h"
#include "util/constant_medium.h"
#include <iomanip>
void bouncing_spheres(){
    hittable_list world;
    shared_ptr<check_texture> check = make_shared<check_texture>(0.32,color(.2,.3,.1),color(0.9,0.9,0.9));
    auto material_ground = make_shared<lambertian>(check);
    world.add(make_shared<sphere>(Point3(0,-1000,0),1000,material_ground));

    for(int a =-11;a<11;a++){
        for(int b=-11;b<11;b++){
            double choose_mat = random_double();
            Point3 center(a+0.9*random_double(),0.2,b+0.9*random_double());
            if((center - Point3(4,0.2,0)).length() > 0.9){
                shared_ptr<material> sphere_material;
                if(choose_mat < 0.8){
                    //diffuse
                    color albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    Point3 center2 = center+vec3(0,random_double(0,0.5),0);
                    world.add(make_shared<sphere>(center,center2,0.2,sphere_material));
                }
                else if(choose_mat < 0.95){
                    //metal
                    color albedo = color::random(0.5,1);
                    double fuzz = random_double(0,0.5);
                    sphere_material =make_shared<metal>(albedo,fuzz);
                    world.add(make_shared<sphere>(center,0.2,sphere_material));
                }
                else{
                    //glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center,0.2,sphere_material));
                }
            }
        }
    }
    
    auto material1 = make_shared<dielectric>(1.50);
    world.add(make_shared<sphere>(Point3(0,1,0),1.0,material1));
    auto material2 = make_shared<lambertian>(color(0.4,0.2,0.1));
    world.add(make_shared<sphere>(Point3(-4,1,0),1.0,material2));
    auto material3 = make_shared<metal>(color(0.7,0.6,0.5),0.0);
    world.add(make_shared<sphere>(Point3(4,1,0),1.0,material3));

    // double R = std::cos(pi/4);
    // auto material_left = make_shared<lambertian>(color(0,0,1));
    // auto material_right=make_shared<lambertian>(color(1,0,0));
    // world.add(make_shared<sphere>(Point3(-R , 0 , -1), R, material_left));
    // world.add(make_shared<sphere>(Point3(R,0,-1),R,material_right));
    bvh_node bvh_root(world.objects,0,world.objects.size()-1);
    camera  cam;
    cam.background        = color(0.70, 0.80, 1.00);
    cam.aspect_ratio=16.0/9.0;
    cam.image_width = 1200;
    cam.samples_per_pixel=100;
    cam.max_depth=50;
    cam.vfov = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat = Point3(0,0,0);
    cam.vup = vec3(0,1,0);
    cam.defocus_angle = 0.6;
    cam.focus_dist=10.0;
    cam.render(bvh_root);
    
}
void checkered_sphere(){
    hittable_list world;
    auto checker = make_shared<check_texture>(0.32,color(.2,.3,.1),color(.9,.9,.9));
    world.add(make_shared<sphere>(Point3(0,-10,0),10,make_shared<lambertian>(checker)));
    world.add(make_shared<sphere>(Point3(0,10,0),10,make_shared<lambertian>(checker)));
    camera cam;
    cam.background        = color(0.70, 0.80, 1.00);
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat = Point3(0,0,0);
    cam.vup = vec3(0,1,0);
    cam.defocus_angle = 0;
    cam.render(world);
}
void earth(){
    hittable_list world;
    shared_ptr<image_texture> earth_texture = make_shared<image_texture>("../images/earthmap.jpg");
    shared_ptr<lambertian> earth_surface = make_shared<lambertian>(earth_texture);
    shared_ptr<sphere> globe = make_shared<sphere>(Point3(0,0,0),2,earth_surface);
    world.add(globe);
    camera cam;
    cam.background        = color(0.70, 0.80, 1.00);
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.vfov = 20;
    cam.lookfrom = Point3(0,0,12);
    cam.lookat = Point3(0,0,0);
    cam.vup = vec3(0,1,0);
    cam.defocus_angle = 0;
    cam.render(world);
}
void perlin_spheres(){
    hittable_list world;
    shared_ptr<noise_texture> pertex = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(Point3(0,-1000,0),1000,make_shared<lambertian>(pertex)));
    world.add(make_shared<sphere>(Point3(0,2,0),2,make_shared<lambertian>(pertex)));
    camera cam;
    cam.background        = color(0.70, 0.80, 1.00);
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = Point3(13,2,3);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}
void quads(){
    hittable_list world;

    // Materials
    auto left_red     = make_shared<lambertian>(color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<quad>(Point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(Point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(Point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(Point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(Point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));
    camera cam;
    cam.background        = color(0.70, 0.80, 1.00);
    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 80;
    cam.lookfrom = Point3(0,0,9);
    cam.lookat   = Point3(0,0,0);
    cam.vup      = vec3(0,1,0);
    cam.defocus_angle = 0;
    cam.render(world);
}
void simple_light(){
    hittable_list world;
    auto pertex = make_shared<noise_texture>(4);
    world.add(make_shared<sphere>(Point3(0,-1000,0),1000,make_shared<lambertian>(pertex)));
    world.add(make_shared<sphere>(Point3(0,2,0),2,make_shared<lambertian>(pertex)));

    auto difflight = make_shared<diffuse_light>(color(4,4,4));
    world.add(make_shared<quad>(Point3(3,1,-2),vec3(2,0,0),vec3(0,2,0),difflight));
    world.add(make_shared<sphere>(Point3(0,7,0),2,difflight));
    camera cam;
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth = 50;
    cam.background = color(0,0,0);
    cam.vfov = 20;
    cam.lookfrom = Point3(26,3,6);
    cam.lookat = Point3(0,2,0);
    cam.vup = vec3(0,1,0);
    cam.defocus_angle = 0;
    cam.render(world);
}
void cornell_box(){
    hittable_list world;
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));
    world.add(make_shared<quad>(Point3(555,0,0),vec3(0,555,0),vec3(0,0,555),green));
    world.add(make_shared<quad>(Point3(0,0,0),vec3(0,555,0),vec3(0,0,555),red));
    world.add(make_shared<quad>(Point3(343,554,332),vec3(-130,0,0),vec3(0,0,-105),light));
    world.add(make_shared<quad>(Point3(0,0,0),vec3(555,0,0),vec3(0,0,555),white));
    world.add(make_shared<quad>(Point3(555,555,555),vec3(-555,0,0),vec3(0,0,-555),white));
    world.add(make_shared<quad>(Point3(0,0,555),vec3(555,0,0),vec3(0,555,0),white));
    shared_ptr<hittable> box1 = box(Point3(0,0,0), Point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));
    world.add(box1);

    shared_ptr<hittable> box2 = box(Point3(0,0,0), Point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));
    world.add(box2);
    bvh_node bvh_root(world.objects,0,world.objects.size()-1);
    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 1000;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = Point3(278, 278, -800);
    cam.lookat   = Point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(bvh_root);
}
void cornell_smoke(){
    hittable_list world;
    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(Point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(Point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(Point3(113,554,127), vec3(330,0,0), vec3(0,0,305), light));
    world.add(make_shared<quad>(Point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(Point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(Point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    shared_ptr<hittable> box1 = box(Point3(0,0,0), Point3(165,330,165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265,0,295));

    shared_ptr<hittable> box2 = box(Point3(0,0,0), Point3(165,165,165), white);
    box2 = make_shared<rotate_y>(box2, -18);
    box2 = make_shared<translate>(box2, vec3(130,0,65));

    world.add(make_shared<constant_medium>(box1, 0.01, color(0,0,0)));
    world.add(make_shared<constant_medium>(box2, 0.01, color(1,1,1)));
    bvh_node bvh_root(world.objects,0,world.objects.size()-1);
    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = Point3(278, 278, -800);
    cam.lookat   = Point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(bvh_root);
}
void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    hittable_list boxes1;
    auto ground = make_shared<lambertian>(color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(Point3(x0,y0,z0), Point3(x1,y1,z1), ground));
        }
    }

    hittable_list world;

    world.add(make_shared<bvh_node>(boxes1));

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    world.add(make_shared<quad>(Point3(123,554,147), vec3(300,0,0), vec3(0,0,265), light));

    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + vec3(30,0,0);
    auto sphere_material = make_shared<lambertian>(color(0.7, 0.3, 0.1));
    world.add(make_shared<sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<sphere>(Point3(260, 150, 45), 50, make_shared<dielectric>(1.5)));
    world.add(make_shared<sphere>(
        Point3(0, 150, 145), 50, make_shared<metal>(color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<sphere>(Point3(360,150,145), 70, make_shared<dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(Point3(0,0,0), 5000, make_shared<dielectric>(1.5));
    world.add(make_shared<constant_medium>(boundary, .0001, color(1,1,1)));

    auto emat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    world.add(make_shared<sphere>(Point3(400,200,400), 100, emat));
    auto pertext = make_shared<noise_texture>(0.2);
    world.add(make_shared<sphere>(Point3(220,280,300), 80, make_shared<lambertian>(pertext)));

    hittable_list boxes2;
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<sphere>(Point3::random(0,165), 10, white));
    }

    world.add(make_shared<translate>(
        make_shared<rotate_y>(
            make_shared<bvh_node>(boxes2), 15),
            vec3(-100,270,395)
        )
    );
    camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = color(0,0,0);

    cam.vfov     = 40;
    cam.lookfrom = Point3(478, 278, -600);
    cam.lookat   = Point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world);
}

int main(){
    cornell_box();

    return 0;
}