#ifndef RTV_H
#define RTV_H
#include "vec3.h"
#include <vector>
enum class RTV
{
    RATE1X1 = 1,
    RATE1X2,
    RATE4X4,
    RATE16X16
};

namespace RTVOFFSET
{
    static int sizeX[5] = {0, 1, 1, 4, 16};
    static int sizeY[5] = {0, 1, 2, 4, 16};
}


inline bool tryChose(int x, int y, int rate, int image_width, int image_height, int* belog)
{
    using namespace RTVOFFSET;
    for(int j = y; j < y + sizeY[rate]; j++)
    {
        for(int i = x; i < x + sizeX[rate]; i++)
        {
            if(i >= image_width || j >= image_height || 
                belog[j * image_width + i] != 0) return false;
        }
    }
    return true;
}

inline void setMask(std::vector<std::vector<std::pair<int, int>>>& mask, int image_width, int image_height,
int* belongRTV)
{
    using namespace RTVOFFSET;
    for(int j = 100; j < 700; j++)
    {
        for(int i = 100; i < 700; i++)
        {
            mask[(int)RTV::RATE1X1].push_back(std::make_pair(i, j));
            belongRTV[j * image_width + i] = (int)RTV::RATE1X1; 
        }
    }
    for(int j = 0; j < image_height; j++)
    {
        for(int i = 0; i < image_width; i++)
        {
            for(int k = 3; k >= 1; k--)
            {
                if(!tryChose(i, j, k, image_width, image_height, belongRTV))
                {
                    continue;
                }
                else
                {
                    mask[k].push_back(std::make_pair(i, j));
                    for(int l = 0; l < sizeY[k]; l++)
                    {
                        for(int m = 0; m < sizeX[k]; m++)
                        {
                            belongRTV[(j + l) * image_width + (i + m)] = k;
                        }
                    }
                }
            }
        }
    }
    
    for(int i = 0; i < image_width * image_height; i++)
        {
            if(belongRTV[i] == 0)
            {
                std::cerr << "have one belong to 0 " << i << "\n";
                exit(1);
            }
        }
}
#endif