#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>


int main()
{
//TEST CHARLY
    Image image = Image();
    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");

    image.imwrite("test.jpg");

    RegionFill region;

    region.set_image(&image);
    region.fill_border();


    for(int r = 0 ; r < region.get_border_size(); ++r)
        std::cout<<region.get_border(r).coord<<std::endl;

    return 0;
}
