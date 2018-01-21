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
    /**Image de test */
//    image.imread("../../images/originales/oval_petit.png","../../images/mask/oval_petit_mask.png");
    //        image.imread("../../images/originales/oval_grand.png","../../images/mask/oval_grand_mask.png");
    //        image.imread("../../images/originales/linc.jpg","../../images/mask/linc-mask.bmp");
    //        image.imread("../../images/originales/fillorder.png","../../images/mask/fillorder_mask.png");
    //        image.imread("../../images/originales/bungee0.png","../../images/mask/bungee0-mask.bmp"); //Not working - malloc corruption ...

    /** Image réelles */
    //        image.imread("../../images/originales/trolltunga.jpg","../../images/mask/trolltunga_mask.bmp");
    //        image.imread("../../images/originales/lakeandballoon.jpg","../../images/mask/lakeandballoon_mask.bmp"); //only balloon
    //        image.imread("../../images/originales/lakeandballoon.jpg","../../images/mask/lakeandballoon_mask2.bmp"); //ballon + ballon in water
    image.imread("../../images/originales/text.jpg","../../images/mask/text-mask.bmp");


    /** Create the instance for the algorithm */
    RegionFill region = RegionFill(3,5);
    /** Furnish the Image containing image and mask to the algorithm */
    region.set_image(&image);
    /** Run the Object Filling algorithm */
    region.run();
    /** Save the final result */
    image.imwrite("resultOvalTest9.png");

    ~image;
    //    ~region;

    return 0;
}
