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
#include <chrono>

int main()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();



    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();



    /** Read Image */
    Image image = Image();
    /**Image de test */
//        image.imread("../../images/originales/oval_petit.png","../../images/mask/oval_petit_mask.png");
//    image.imread("../../images/originales/oval_grand.png","../../images/mask/oval_grand_mask.png");
        image.imread("../../images/originales/linc.jpg","../../images/mask/linc-mask.bmp");
//        image.imread("../../images/originales/fillorder.png","../../images/mask/fillorder_mask.png");
//        image.imread("../../images/originales/bungee0.png","../../images/mask/bungee0-mask.bmp"); //Not working - malloc corruption ...
//image.imread("../../images/originales/concave.bmp","../../images/mask/concave.bmp");
    /** Image réelles */
//            image.imread("../../images/originales/trolltunga.jpg","../../images/mask/trolltunga_mask.bmp");
//        image.imread("../../images/originales/lakeandballoon.jpg","../../images/mask/lakeandballoon_mask.bmp"); //only balloon
    //    image.imread("../../images/originales/lakeandballoon.jpg","../../images/mask/lakeandballoon_mask2.bmp"); //ballon + ballon in water
//        image.imread("../../images/originales/text.jpg","../../images/mask/text-mask.bmp");
    //    image.imread("../../images/originales/bruandet.png","../../images/mask/bruandet-mask.bmp");
    //    image.imread("../../images/originales/bruandet2.png","../../images/mask/bruandet2-mask.bmp");
//        image.imread("../../images/originales/Young_jaws_features.png","../../images/mask/Young_jaws_features.bmp");
//    image.imread("../../images/originales/plane.jpg","../../images/mask/plane.bmp");



    /** Create the instance for the algorithm */
    RegionFill region = RegionFill(7,7);
    /** Furnish the Image containing image and mask to the algorithm */
    region.set_image(&image);
    /** Run the Object Filling algorithm */
    region.run();
    /** Save the final result */
    image.imwrite("test.png");

    ~image;

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    int time = std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
    std::cout << "Execution Time :" << std::endl;
    int secondes = time%60;
    int minutes = (time-secondes)/60.0f;
    float heures = (time-secondes-60.0f*minutes)/60.0f;
    std::cout << "Heure : " << heures << " Minutes : " << minutes << " secondes : " << secondes << std::endl;

    return 0;
}
