/** Image.hpp
 * Inpainting project
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
#pragma once

#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>

enum
{
//    IN, BORDER, UPDATED, SOURCE
    SOURCE,
    UPDATED,
    BORDER,
    IN,
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
    cv::Mat  image() const;
    /** Get Masj */
    cv::Mat  mask() const;
    /** Get alpha Map */
    cv::Mat  alpha() const;

    /** Set Image */
    cv::Mat& image();

    /** Get Image value at pixel u,v */
    cv::Vec3b get_image_pixel(int u, int v) const;
    /** Get Mask value at pixel u,v */
     uchar get_mask_pixel(int u, int v) const;
    /** Get alpha value at pixel u,v */
    uchar get_alpha_pixel(int u, int v) const;

    /** Set Image value at pixel u,v */
    cv::Vec3b& set_image_pixel(int u, int v);
    /** Set Alpha value at pixel u,v */
    uchar& set_alpha_pixel(int u, int v);

    /** Get image rows */
    int const& get_rows() const;
    /** GEt image cols */
    int const& get_cols() const;




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
