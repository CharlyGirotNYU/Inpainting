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



    // Point ou data term non nul
//    (199, 122) (200, 122) (201, 122) (242, 129) (243, 129)  (369, 140) (370, 140) (370, 146)
    region.compute_data_term(cv::Point2i(67,330));

//    region.test_compute_data_term();
//    std::cout << "SOURCE " << SOURCE << std::endl ;
//    std::cout << "UPDATED " << UPDATED << std::endl ;
//    std::cout << "BORDER " << BORDER << std::endl ;
//    std::cout << "IN " << IN << std::endl ;

    //TEST DIF



    return 0;
}
