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




    /** Update alpha  after a patch copy centered at bp */
    void update_alpha(cv::Point2i bp,int sizex,int sizey);
    /** Update the border */
    void update_border(cv::Point2i point, int status); // Can we rename it : update_alpha_status ? cause it's not only updating the border points
    /** Return true if the point if a new border, else false */
    bool is_new_border(int u, int v);


    /** Compute Isophote of the image
    *   -L : Luminance of the input image
    *   -alpha : Threshold of isophotes */
    cv::Point compute_isophotes(cv::Point2i p);

    /** Init confidence term Mat*/
    void init_confidence();

    /** Compute confidence term */
    float compute_confidence(cv::Point2i p);

    /** Compute data_term */

    float compute_data_term(cv::Point p);

    /** Compute the vector n_p from the point p and his nieghbors */
    cv::Point2f compute_vector_normal(cv::Point p,  cv::Mat p_neighbors);


    /** Test de la fonction compute data term */
    void test_compute_data_term();


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

    /** Compute distance (SSD) between 2 RGB patch of size_patch in the CIELab Color Space */
//    float compute_patch_SSD_LAB(cv::Mat A, cv::Mat B);

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


    /** Get border point by coordinates */ //NOT IMPLEMENTED ?
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

    /** Patch */
    //    cv::Mat patch; !! attention aux conflits de noms


};

#endif // REGIONFILL_HPP
