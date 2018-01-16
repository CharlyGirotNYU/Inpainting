#ifndef PATCH_HPP
#define PATCH_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "image.hpp"

class patch
{
public:
    patch();
    patch(Image *im,  int x, int y ,cv::Point2i center,bool source);
    patch(Image *im, int x, int y);
    void set_center_and_fill(cv::Point2i center,bool source);


    void mask(patch P);

    bool is_whole_patch_source();

    float compute_distance_SSD_LAB(patch B);

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
