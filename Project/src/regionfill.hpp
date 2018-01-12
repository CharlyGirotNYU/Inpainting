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
    float priority;
};

class RegionFill
{
public:
    /** Empty Constructor */
    RegionFill();
    /** get and fill the border */
    void init_border();


    /** Get current border */
    std::vector<border_point> get_border() const;
    /** Set the current border */
    void set_border(std::vector<border_point>& b);

    /** Get current border by index */
    border_point get_border(unsigned int i) const;
    /** Get border point by coordinates */
    border_point get_border(cv::Point2i p);

    /** Set the pointer to the parent Image */
    void set_image(Image* image);
    /** Get Original Image */
    Image get_image() const;

    /** Get size of the border */
    int get_border_size() const;

    /** Update alpha  after a patch copy centered at bp */
    void update_alpha();
    /** Update the border */
    void update_border(border_point point, int status); // Can we rename it : update_alpha_status ? cause it's not only updating the border points
    /** Return true if the point if a new border, else false */
    bool is_new_border(int u, int v);


    /** Compute Isophote of the image
    *   -L : Luminance of the input image
    *   -alpha : Threshold of isophotes */
    void compute_isophotes();

    /** Init confidence term Mat*/
    void init_confidence();

    /** Compute confidence term */
    float compute_confidence(cv::Point2i p);

    /** Compute data_term */
    float compute_data_term(cv::Point2i p);

    /** Compute priority of a border point */
    void compute_priority();

    /** Running through the patches */
    cv::Point2i running_through_patches();

    /** Check if the whole image has been processed */
    bool whole_image_processed();

    /** find exemplar patch :
     * We search in the source region for the patch which is most similar to Pp.
     * d(Pp,Pq) is the sum of squaerd differences SSD of the ALREADY filled pixels in the two patches
     * We use the CIE Lab colour space because of its property of perceptual information
     * Receive : center point of the reference patch P
     * Return : the center point of the exemplar patch Q */
    cv::Point2i find_exemplar_patch(cv::Point2i p);

    /** Propagate texture from exemplar Patch to patch on the border */
    void propagate_texture(cv::Point2i p, cv::Point2i q);

    /** Run the algo */
    void run();


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
    //    cv::Mat patch; !! attention aux conflits de noms


    // definiton de alpha ici ?

};

#endif // REGIONFILL_HPP
