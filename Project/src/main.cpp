/** Main.cpp
 * Inpainting Project
 * Developed by Di Folco Maxime, Girot Charly, Jallais Maëllis
 * January 2018
 * CPE Lyon - 5ETI IMI
 * End of semester Project
 *
 * Based on the work and paper :
 * Criminisi, Antonio, Patrick Pérez, and Kentaro Toyama.
 * "Region filling and object removal by exemplar-based image inpainting."
 * IEEE Transactions on image processing 13.9 (2004): 1200-1212.
 * */

#include "image.hpp"
#include "regionfill.hpp"
#include<iostream>
#include<stdlib.h>
#include "patch.hpp"

int main()
{
    /** Read Image */
    Image image = Image();
//    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln2.png");
//    image.imread("../../images/mask_oval.png","../../images/masked_oval.png");
//    image.imread("../../images/a.png","../../images/b.png");
//    image.imread("../../images/test_filling_order.png","../../images/mask_test_filling_order_carre.png");
//    image.imread("../../images/test_priority.png","../../images/test_priority_mask.png");
//    image.imread("../../images/c.png","../../images/d.png");
    image.imread("../../images/trolltunga.jpg","../../images/trolltunga_mask.bmp");
//    image.imread("../../images/lakeandballoon.jpg","../../images/lakeandballoon_mask.bmp");
//    image.imread("../../images/Criminisi.jpg","../../images/Criminisi-mask.bmp");

    /** Create the instance for the algorithm */
    RegionFill region = RegionFill();
    /** Furnish the Image containing image and mask to the algorithm */
    region.set_image(&image);
    /** Run the Object Filling algorithm */
    region.run();

    return 0;
}
