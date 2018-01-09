/* Image.hpp */

#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>

enum
{
    IN, BORDER, UPDATED, SOURCE
};

class Image : public cv::Mat
{
public:
    Image();


    /** Read original image and mask then compute alphas <==> Set Image and Set Alpha */
    void imread(std::string image_path, std::string mask_path);

    /** Write an image */
    void imwrite(std::string filename);

    /** Get Image */
    cv::Mat const& image() const;
    /** Get Masj */
    cv::Mat const& mask() const;
    /** Get alpha Map */
    cv::Mat const& alpha() const;
    /** Get Image value at pixel u,v */
    uchar const& image(int u, int v) const;
    /** Get Mask value at pixel u,v */
     uchar const& mask(int u, int v) const;
    /** Get alpha value at pixel u,v */
    uchar const& alpha(int u, int v) const;

    /** Get image rows */
    int const& get_rows() const;
    /** GEt image cols */
    int const& get_cols() const;

    /** Set Image value at pixel u,v */
    uchar& image(int u, int v);
    /** Set Alpha value at pixel u,v */
    uchar& alpha(int u, int v);

    /** Return number of pixels surrounding the current pixel that are outside the mask  */
    int num_outside_mask(int u, int v);



private:
    /** Store the image to fill/restore */
    cv::Mat image_data;
    /** Store the mash */
    cv::Mat mask_data;
    /** Store the alphas : 0->region to fill 1->border of the region 2->region already filled 3-> image source */
    cv::Mat alpha_data;

    /** Image length */
    int Nu;
    /** Image width */
    int Nv;
};



#endif
