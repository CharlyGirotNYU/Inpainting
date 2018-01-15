#ifndef PATCH_HPP
#define PATCH_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include "image.hpp"

class patch
{
public:
    patch();
    patch(Image *im,  int size,cv::Point2i center,bool source);
    patch(Image *im, int size);
    void set_center_and_fill(cv::Point2i center,bool source);


    void mask(patch a);

    bool is_whole_patch_source();

    float compute_distance_SSD_LAB(patch B);

    cv::Vec3b get_pixel(int i, int j){return patch_data.at<cv::Vec3b>(i,j);}


private:

    Image* image_data;
    cv::Mat patch_data;
    cv::Point2i center_data;
    int patch_size;
};

#endif // PATCH_HPP
