#include "regionfill.hpp"

RegionFill::RegionFill()
{
}


/** get and fill the border */
std::vector<border_point> RegionFill::fill_border()
{
    for(int i=0; i<im->rows; ++i)
    {
        for(int j=0; i<im->cols; ++j)
        {
            //if(im->alpha(i,j) == 1)
            {
                border_point point;
                point.confidence = 0.f;
                point.coord = cv::Point2i(i,j);
                border.push_back(point);
            }
        }
    }
}

/** update the border */
std::vector<border_point> RegionFill::update_border()
{
    //TO DO
}

/** Get Original Image */
Image RegionFill::get_image() const
{
    return *im;
}

/** Get current border */
std::vector<border_point> RegionFill::get_border() const
{
    return border;
}


/** Set the pointer to the parent Image */
void RegionFill::set_image(Image* image)
{
    im = image;
}

/** Set the current border */
void RegionFill::set_border(std::vector<border_point>& b)
{
    border = b;
}


void RegionFill::compute_isophotes( float alpha)
{

}
