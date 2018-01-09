#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>


int main()
{
//TEST CHARLY
    Image image = Image();
//    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");
    image.imread("../../images/oval.png","../../images/masked_oval.png");

    image.imwrite("test.jpg");

    RegionFill region = RegionFill();
    region.set_image(&image);
    region.compute_isophotes(0.8);


    //TEST DIF



    return 0;
}
