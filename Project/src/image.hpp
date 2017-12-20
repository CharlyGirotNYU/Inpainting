/* Image.hpp */

#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>


class Image : public cv::Mat
{
public:
    Image();

    /** Read original image and mask then compute alphas */
    void imread(std::string image_path, std::string mask_path);

    /** Write an image */
    void imwrite(std::string filepath);

private:
    /** Store the image to fill/restore */
    cv::Mat image;
    /** Store the alphas : 0->regoin to fill 1-> border of the region 2->OK Region */
    cv::Mat alpha;

};



#endif
