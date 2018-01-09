#ifndef REGIONFILL_HPP
#define REGIONFILL_HPP

#include "image.hpp"

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>


struct border_point
{
    cv::Point2i  coord;
    float        confidence;
};

class RegionFill
{
public:
    /** Empty Constructor */
    RegionFill();
    /** get and fill the border */
    void fill_border();
    /** Get Original Image */
    Image get_image() const;
    /** Get current border */
    std::vector<border_point> get_border() const;
    /** update the border */
    void update_border();

    /** Set the pointer to the parent Image */
    void set_image(Image* image);
    /** Set the current border */
    void set_border(std::vector<border_point>& b);



    /** Compute Isophote of the image
    *   -L : Luminance of the input image
    *   -alpha : Threshold of isophotes */
    void compute_isophotes(float alpha);


    //    /** Fill the region / Get the region */
        //cv::Mat fill_region();

    //    /** Get current region */
    //    cv::Mat region() const;
//    /** Get offset on x of the box of the region */
//    int offset_x() const;
//    /** Get offset on y of the box of the region */
//    int offset_y() const;
//    /** Set the current region  */
//    cv::Mat& region();
//    /** Set the offset on x of the box of the region */
//    int& offset_x();
//    /** Set the offset on y of the box of the region */
//    int& offset_y();




private:
    /** Pointer to the original image */
    Image *im;
    /** Storage of the border */
    std::vector<border_point> border;

    /** Storage of the isophotes */
    cv::Mat isophotes_data_magnitude;
    cv::Mat isophotes_data_orientation;

//    /** Storage of the region to fill */
    //cv::Mat region;

//    /** Offset on x axis of the region */
//    int offset_x;
//    /** Offset on y axis of the region */
//    int offset_y;


    // definiton de alpha ici ?

};

#endif // REGIONFILL_HPP
