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
    //        image.imread("../../images/lincoln.jpg","../../images/mask_lincoln2.png");
//        image.imread("../../images/mask_oval.png","../../images/masked_oval.png");
//        image.imread("../../images/a.png","../../images/b.png");
    //image.imread("../../images/test_filling_order_carre.png","../../images/mask_test_filling_order_carre.png");
    image.imread("../../images/test_priority.png","../../images/test_priority_mask.png");
//    image.imread("../../images/c.png","../../images/d.png");
    image.imwrite("test.bmp");

    /** Create the instance for the algorithm */
    RegionFill region = RegionFill();
    /** Furnish the Image containing image and mask to the algorithm */
    region.set_image(&image);
    /** Run the algorithm */
        region.run();


    region.init_confidence();
    region.init_border();

    std::cout << region.compute_data_term(cv::Point2i(33,113)) << std::endl;
    std::cout << region.compute_confidence(cv::Point2i(33,113)) << std::endl;
    std::cout << region.compute_data_term(cv::Point2i(195,132)) << std::endl;
    std::cout << region.compute_confidence(cv::Point2i(195,132)) << std::endl;



//    std::cout << region.compute_data_term(cv::Point2i(33,113)) << std::endl;
//    std::cout << region.compute_confidence(cv::Point2i(195,132)) << std::endl;


    //    region.init_confidence();
    //    region.init_border();
    //    cv::Point p =cv::Point2i(48,53);
    //    cv::Point p1 =cv::Point2i(64,87);
    //    std::cout<< region.compute_data_term(p)<<" "<<region.compute_confidence(p)<<std::endl;
    //    std::cout<< region.compute_data_term(p1)<<" "<<region.compute_confidence(p1)<<std::endl;


    //    region.init_border();
    //    region.compute_priority();
    //    cv::namedWindow("Coucou",cv::WINDOW_NORMAL);
    //    cv::imshow("Coucou",region.get_image().image());
    //    cv::waitKey(0);
    //    //Test distance

    //    patch P(&image,9,9,cv::Point2i(267,373),false);
    //    patch Q(&image,9,9,cv::Point2i(230,390),false);
    //    cv::imshow("P", P.get_patch());
    //    cv::imshow("Q", Q.get_patch());
    //    cv::waitKey(0);
    //    std::cout << P.compute_distance_SSD_LAB(Q) << std::endl;
    //    std::cout << P.is_whole_patch_source() << std::endl;
    //    std::cout << Q.is_whole_patch_source() << std::endl;





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
