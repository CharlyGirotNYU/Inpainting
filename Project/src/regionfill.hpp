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
    /** Fill the region / Get the region */
    cv::Mat fill_region();
    /** get and fill the border */
    std::vector<border_point> fill_border();

    /** Get Original Image */
    Image image() const;
    /** Get current region */
    cv::Mat region() const;
    /** Get current border */
    std::vector<border_point> border() const;
    /** Get offset on x of the box of the region */
    int offset_x() const;
    /** Get offset on y of the box of the region */
    int offset_y() const;

    /** Set the pointer to the parent Image */
    void image(Image* im);
    /** Set the current region  */
    cv::Mat& region();
    /** Set the current border */
    std::vector<border_point>&  border();
    /** Set the offset on x of the box of the region */
    int& offset_x();
    /** Set the offset on y of the box of the region */
    int& offset_y();




private:
    /** Pointer to the original image */
    Image *im;
    /** Storage of the region to fill */
    cv::Mat region;
    /** Storage of the border */
    std::vector<border_point> border;
    /** Offset on x axis of the region */
    int offset_x;
    /** Offset on y axis of the region */
    int offset_y;

};

#endif // REGIONFILL_HPP
