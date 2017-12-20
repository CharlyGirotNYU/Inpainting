/* Image.hpp */

#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>

enum
{
    OUT,
    UPDATED,
    BORDER,
    IN
};

class Image : public cv::Mat
{
public:
    Image();

    /** Read original image and mask then compute alphas <==> Set Image and Set Alpha */
    void imread(std::string image_path, std::string mask_path);

    /** Write an image */
    void imwrite(std::string filepath);

    /** Get Image */
    cv::Mat const& image() const;
    /** Get alpha Map */
    cv::Mat const& alpha() const;
    /** Get Image value at pixel u,v */
    int const& image(int u, int v) const;
    /** Get alpha value at pixel u,v */
    int const& alpha(int u, int v) const;

    /** Set Image value at pixel u,v */
    int& image(int u, int v);
    /** Set Alpha value at pixel u,v */
    int& alpha(int u, int v);

    /** Return if a pixel belong to the border */
    bool is_border(int u, int v);


private:
    /** Store the image to fill/restore */
    cv::Mat image_data;
    /** Store the alphas : 0->region to fill 1->border of the region 2->region already filled 3-> image source */
    cv::Mat alpha_data;
};



#endif
