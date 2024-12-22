#ifndef RTW_STB_IMAGE_H
#define RTW_STB_IMAGE_H
// Disable strict warnings for this header from the Microsoft Visual C++ compiler.
#ifdef _MSC_VER
    #pragma warning(push,0);
#endif
#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include "extern/stb_image.h"
#include <cstdlib>
#include <iostream>
class rtw_image{
public:
    rtw_image(){}
    rtw_image(const char* image_filename){
        // 从指定的文件加载图像数据。如果定义了 RTW_IMAGES 环境变量，
        // 则只在该目录中查找图像文件。如果找不到图像，
        // 首先从当前目录开始搜索指定的图像文件，然后在 images/ 子目录中搜索，
        // 接着是 父目录 的 images/ 子目录，然后是 那个父目录的父目录，依此类推，
        // 向上搜索六个级别。如果图像没有成功加载，
        // width() 和 height() 将返回 0。
        std::string filename = std::string(image_filename);
        auto imagedir = getenv("RTW_IMAGES");

        if(imagedir && load(std::string(imagedir) + "/" + filename)) return;
        if(load(filename)) return;
        if(load("images/"+filename)) return;
        if(load("../images/"+filename)) return;
        if(load("../../images/"+filename)) return;
        if(load("../../../images/"+filename)) return;
        if(load("../../../../images/"+filename)) return;
        if(load("../../../../../images/"+filename)) return;
        if(load("../../../../../../images/"+filename)) return;

        std::cerr<<"ERROR: Could not load image file'"<<image_filename<<"'.\n";
    }
    bool load(const std::string& filename){
        // 从给定的文件名加载线性（gamma=1）图像数据。
        // 如果加载成功，返回 true。
        // 结果数据缓冲区包含第一个像素的三个 [0.0, 1.0] 
        // 范围内的浮点值（首先是红色，然后是绿色，然后是蓝色）。
        // 像素是连续的，从图像的宽度方向从左到右，接着是下一行，
        // 直到整个图像的高度。
        int n = bytes_per_pixel;
        fdata = stbi_loadf(filename.c_str(),&image_width,&image_height,&n,bytes_per_pixel);
        if (fdata == nullptr) return false;
        bytes_per_scanline = image_width * bytes_per_pixel;
        convert_to_bytes();
        return true;
    }
    int width() const {return (fdata == nullptr) ? 0 : image_width;}
    int height() const {return (fdata == nullptr) ? 0 : image_height;}
    const unsigned char* pixel_data(int x,int y) const{
        static unsigned char magenta[] = {255,0,255};
        if(bdata == nullptr) return magenta;
        x = clamp(x,0,image_width);
        y = clamp(y,0,image_height);
        return bdata + y * bytes_per_scanline + x * bytes_per_pixel;
    }
private:
    const int bytes_per_pixel = 3;
    float    *fdata = nullptr; // Linear floating point pixel data 每个像素的0~1的值
    unsigned char *bdata = nullptr; //// Linear 8-bit pixel data 转换为8bit的颜色值
    int       image_width = 0;
    int       image_height = 0;
    int       bytes_per_scanline = 0;
    static int clamp(int x,int low,int high){
        if (x<low) return low;
        if (x<high) return x;
        return high-1;
    }
    static unsigned char float_to_byte(float value){
        if (value <= 0.0) return 0;
        if (value >= 1.0) return 255;
        return static_cast<unsigned char>(256.0 * value);//这里希望，比如0.99可以转换为255
    }
    void convert_to_bytes(){
        // Convert the linear floating point pixel data to bytes, storing the resulting byte
        // data in the `bdata` member.
        int total_bytes = image_width * image_height * bytes_per_pixel;
        bdata = new unsigned char[total_bytes];
        unsigned char *bptr = bdata;
        float *fptr = fdata;
        for (int i=0;i<total_bytes;i++,bptr++,fptr++){
            *bptr = float_to_byte(*fptr);
        }
    }
};
// Restore MSVC compiler warnings
#ifdef _MSC_VER
    #pragma warning (pop)
#endif

#endif