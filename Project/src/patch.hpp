/** patch.hpp
 * Inpainting project
 * Developed by Di Folco Maxime, Girot Charly, Jallais Maëliss
 * January 2018
 * CPE Lyon - 5ETI IMI
 * End of semester Project
 *
 * Based on the work and paper :
 * Criminisi, Antonio, Patrick Pérez, and Kentaro Toyama.
 * "Region filling and object removal by exemplar-based image inpainting."
 * IEEE Transactions on image processing 13.9 (2004): 1200-1212.
 * */

#ifndef PATCH_HPP
#define PATCH_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "image.hpp"

class patch
{
public:
    /** Default constructor */
    patch();

    /** Constructor :: all parameters known */
    patch(Image *im,  int x, int y ,cv::Point2i center,bool source);

    /** Constructor :: only size and image related to is known */
    patch(Image *im, int x, int y);

    /** Positionning the center of the patch and fill patch_data */
    bool set_center_and_fill(cv::Point2i center,bool source);

    /** Check if the whole patch_data is in the source */
    bool is_whole_patch_source();

    /** Compute distance between two patch_data for the intersection of the 2 patch_data contained in the source
     * SSD Computed in Lab Color Space */
    float compute_distance_SSD_LAB(patch B);

    /** Get pixel value of the patch data at (i,j)*/
    cv::Vec3b get_pixel(int i, int j){return patch_data.at<cv::Vec3b>(i,j);}
    /** Get Patch Mat */
    cv::Mat get_patch() const {return patch_data;}
    /** Get whole image containing the patch */
    cv::Mat get_image() const {return image_data->image();}
    /** Get whole alpha Mat */
    cv::Mat get_alpha() const {return image_data->alpha();}

    /** Get center of the patch */
    cv::Point2i get_center() {return center_data; }

    cv::Point2i get_size(){return cv::Point2i(sizex,sizey);}

private:

    Image* image_data;
    cv::Mat patch_data;
    cv::Point2i center_data;
    int sizex;
    int sizey;
};

#endif // PATCH_HPP
