#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>
#include "patch.hpp"

int main()
{
    //TEST CHARLY
    Image image = Image();
    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");
//        image.imread("../../images/mask_oval.png","../../images/masked_oval.png");

    image.imwrite("test.jpg");

    RegionFill region = RegionFill();
    region.set_image(&image);
    std::cout << "main " << image.get_cols() << std::endl;

//    cv::Point a = region.compute_isophotes(cv::Point2i(101,187));
//    std::cout<< a.x << " "<<a.y<<std::endl;
    region.run();

    std::cout << "SOURCE " << SOURCE << std::endl ;
    std::cout << "UPDATED " << UPDATED << std::endl ;
    std::cout << "BORDER " << BORDER << std::endl ;
    std::cout << "IN " << IN << std::endl ;


    //TEST DIF

    //Charly
//    patch p(image.get_image());
//    std::cout << region.whole_image_processed() << std::endl;

    //tester copute priority
    //tester running through patches
    //tester find exemplar patch
    //tester propagate texture

    return 0;
}
