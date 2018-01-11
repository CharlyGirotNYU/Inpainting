#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>


int main()
{
//TEST CHARLY
    Image image = Image();
    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");
//    image.imread("../../images/oval.png","../../images/masked_oval.png");

    image.imwrite("test.jpg");

    RegionFill region = RegionFill();
    region.set_image(&image);
    region.compute_isophotes(0.8);
    region.init_confidence();
    region.fill_border();
    region.compute_confidence(cv::Point2i(258,112));
    region.compute_data_term();
//    std::cout << "SOURCE " << SOURCE << std::endl ;
//    std::cout << "UPDATED " << UPDATED << std::endl ;
//    std::cout << "BORDER " << BORDER << std::endl ;
//    std::cout << "IN " << IN << std::endl ;

    //TEST DIF



    return 0;
}
