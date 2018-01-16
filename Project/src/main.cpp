#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>
#include "patch.hpp"

int main()
{
    /** Disable Output */
//    // get underlying buffer
//    std::streambuf* orig_buf = std::cout.rdbuf();
//    // set null
//    std::cout.rdbuf(NULL);
//    // restore buffer
//    std::cout.rdbuf(orig_buf);

    /** Read Image */
    Image image = Image();
    //    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");
    image.imread("../../images/mask_oval.png","../../images/masked_oval.png");


    image.imwrite("test.jpg");

    /** Create the instance for the algorithm */
    RegionFill region = RegionFill();
    /** Furnish the Image containing image and mask to the algorithm */
    region.set_image(&image);
    /** Run the algorithm */
    region.run();



    return 0;
}


/// Initialize arguments for the padding filter
//       cv::Mat src = image.image();
//      cv::Mat dst = image.image();
//     int top, bottom, left, right;
//     top = (int) (0.05*src.rows); bottom = (int) (0.05*src.rows);
//     left = (int) (0.05*src.cols); right = (int) (0.05*src.cols);
//    top =10; bottom = 10; left = 10; right = 10;
//     cv::Scalar value;
//     cv::RNG rng(12345);
//    value = cv::Scalar( rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255) );
//    cv::copyMakeBorder( image.image(), dst, top, bottom, left, right, cv::BORDER_REPLICATE, value );

//    cv::imshow("A",dst);
//    cv::waitKey(0);
