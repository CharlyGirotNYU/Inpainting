#ifndef REGIONFILL_HPP
#define REGIONFILL_HPP

#include "image.hpp"

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>


struct border_point
{
    cv::Point2i  coord;
    //    float        confidence;
    float  data_term;
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

    /** Get current border by index */
    border_point get_border(unsigned int i) const;
    /** Get size of the border */
    int get_border_size() const;

    /** Update alpha  after a patch copy centered at bp */
    void update_alpha(border_point bp);
    /** Update the border */
    void update_border(border_point point, int status);
    /** Return true if the point if a new border, else false */
    bool is_new_border(int u, int v);

    /** Set the pointer to the parent Image */
    void set_image(Image* image);
    /** Set the current border */
    void set_border(std::vector<border_point>& b);




    /** Compute Isophote of the image
    *   -L : Luminance of the input image
    *   -alpha : Threshold of isophotes */
    void compute_isophotes(float alpha);

    /** Init confidence term Mat*/
    void init_confidence();

    /** Compute confidence term */
    float compute_confidence(cv::Point2i p);

    /** Compute data_term */
    float compute_data_term(cv::Point2i p);

    /** Compute priority of a border point */
    float compute_priority(cv::Point2i p);

    /** Running through the patches */
    cv::Point2i running_trhough_patches();

    /** Check if the whole image has been processed */
    bool whole_image_processed();

    /** Run the algo */
    void run();

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

    /** Storage of the confidence */
    cv::Mat confidence;

    /** Storage of the isophotes */
    cv::Mat isophotes_data_magnitude;
    cv::Mat isophotes_data_orientation;

    /** Size of a patch (width=height)(9*9 in Creiminisi)*/
    int patch_size;

    /** Patch */
    cv::Mat patch;


    // definiton de alpha ici ?

};

#endif // REGIONFILL_HPP
