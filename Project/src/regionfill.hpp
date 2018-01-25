/** regionfillhpp
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

#ifndef REGIONFILL_HPP
#define REGIONFILL_HPP

#include "image.hpp"

#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>


struct border_point
{
    cv::Point2i  coord;
    float priority;
};

class RegionFill
{
public:
    /** Empty Constructor */
    RegionFill();
    ~RegionFill();
    /** Constructor with patch size on x and y */
    RegionFill(int x, int y);

    /** get and fill the border of the mask*/
    void init_border();

    /** Update alpha  after a patch copy centered at bp */
    void update_alpha(cv::Point2i bp,int sizex,int sizey);
    /** Update the border */
    void update_border(cv::Point2i point, int status); // Can we rename it : update_alpha_status ? cause it's not only updating the border points
    /** Return true if the point if a new border, else false */
    bool is_new_border(int u, int v);

    /** Compute Isophote of the image
    *   -L : Luminance of the input image
    *   -alpha : Threshold of isophotes */
    cv::Point2f compute_isophotes(cv::Point2i p);

    /** Init confidence term of the whole image contained in im and store result in confidence*/
    void init_confidence();
    /** Compute confidence term of point using patch arroungd it*/
    float compute_confidence(cv::Point2i p);
    /** Compute data_term of a point using patch arround it*/
    float compute_data_term(cv::Point2i p);
    /** Compute the vector n_p from the point p and his nieghbors */
    cv::Point2f compute_vector_normal(cv::Point p,  cv::Mat p_neighbors);

    /** Compute priority for all point belonging to the border of mask */
    void compute_priority();
    /** Running through patches along the border to find the border point of highest priority */
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
    void propagate_texture(cv::Point2i p, cv::Point2i q, int sizex, int sizey);

    /** Run the algo */
    void run();

    /** Get Original Image */
    Image get_image() const                         {return *im;}
    /** Get current border */
    std::vector<border_point> get_border() const    {return border;}
    /** Get current border by index */
    border_point get_border(unsigned int i) const   {return border[i];}
    /** Get size of the border */
    int get_border_size() const                     {return border.size();}
    /** Set the pointer to the parent Image */
    void set_image(Image* image)                    { im = image;}
    /** Set the current border */
    void set_border(std::vector<border_point>& b)   {border =b;}

    /** Get border point by coordinates */
    border_point get_border(cv::Point2i p);


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
    int patch_size_x;
    int patch_size_y;

};

#endif // REGIONFILL_HPP
