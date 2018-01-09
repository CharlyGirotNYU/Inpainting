#include "regionfill.hpp"

RegionFill::RegionFill()
{
}


/** get and fill the border */
void RegionFill::fill_border() // A tester
{
    for(int i=0; i<im->alpha().rows; ++i)
    {
        for(int j=0; j<im->alpha().cols; ++j)
        {
            if(im->alpha(i,j) == 1)
            {
                border_point point;
                point.confidence = 0.f;
                point.coord = cv::Point2i(i,j);
                border.push_back(point);
            }
        }
    }
}

/** Update alpha  after a patch copy centered at bp */
void RegionFill::update_alpha(border_point bp)
{
    int step = floor(PATCH_SIZE/2);
    // Update alpha of the points belonging to the patch
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            int x = bp.coord.x+i; int y = bp.coord.y+j;
            if(im->alpha(x,y) != SOURCE)
            {
                im->alpha(x,y) = UPDATED;
                border_point new_bp;
                new_bp.coord = cv::Point2i(x,y);
                update_border(new_bp,UPDATED);
            }
        }

    // Update the possible border around the new patch. Only the points that
    //were in the mask can become a border

    //Taking care of the angles
    for (int i=-step-1; i<=step+1; ++i)
    {
        //The line under the patch
        int x = bp.coord.x+i; int y = bp.coord.y-step-1;
        if(im->alpha(x,y) == IN && is_new_border(x,y))
        {
            border_point new_bp;
            new_bp.coord = cv::Point2i(x,y);
            update_border(new_bp,BORDER);
        }

        //The line over the patch
        y = bp.coord.y+step+1;
        if(im->alpha(x,y) == IN && is_new_border(x,y))
        {
            border_point new_bp;
            new_bp.coord = cv::Point2i(x,y);
            update_border(new_bp,BORDER);
        }
    }

    //No need to take care of the angles
    for (int j=-step; j<=step; ++j)
    {
        //The line on the left of the patch
        int x = bp.coord.x-step-1; int y = bp.coord.y+j;
        if(im->alpha(x,y) == IN && is_new_border(x,y))
        {
            border_point new_bp;
            new_bp.coord = cv::Point2i(x,y);
            update_border(new_bp,BORDER);
        }

        //The line on the right of the patch
        x = bp.coord.x+step+1;
        if(im->alpha(x,y) == IN && is_new_border(x,y))
        {
            border_point new_bp;
            new_bp.coord = cv::Point2i(x,y);
            update_border(new_bp,BORDER);
        }
    }
}

/** Return true if the point if a new border, else false */
bool RegionFill::is_new_border(int u, int v)
{
    //A remplacer avec le get quand merge avec Charly
    int Nu= 371;
    int Nv= 432;

    int nb_out = 0;
    //Compute the number of points UPDATED or SOURCE around the neighborhoods
    for(int j=-1; j<=1; j++)
        for(int i=-1; i<=1 ; i++)
        {
            if(!(i==0 && j == 0)) //don't test the center pixel
                if(u+i>=0 && u+i<Nu) //Check if within the image region/size
                    if( v+j<Nv && v+j>=0)
                        if(im->alpha(u+i,v+j) == UPDATED || im->alpha(u+i,v+j) == SOURCE)
                            nb_out++;
        }

    if(nb_out >0 && nb_out<8)
        return true;
    else if(nb_out == 9)
        std::cout << "The point has not been updated whereas all the neighboors have been" << std::endl;
    else return false;
}


/** Update the border according to the status :
    - IN = belongs to the mask - nothing to do
    - BORDER = need to be added to the vector
    - UPDATED = previously BORDER or IN, need to be removed from the vector
    - SRC = nothing to do (theses points never change)

General use : Called during the alpha_data update when a point changes status*/
void RegionFill::update_border(border_point point, int status)
{
    if(status == BORDER)
    {
        border.push_back(point);
        //Update the alpha value
        im->alpha(point.coord.x,point.coord.y) = BORDER;
    }
    else if(status == UPDATED)
    {
        //Update the alpha value
        im->alpha(point.coord.x,point.coord.y) = UPDATED;

        std::vector<border_point>::iterator it;
        it=border.begin();
        for(; it!=border.end(); ++it)
        {
            if((*it).coord == point.coord)
            {
                border.erase(it);
            }
        }
    }

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

/** Get current border by index */
border_point RegionFill::get_border(unsigned int i) const
{
    return border[i];
}

/** Get size of the border */
int RegionFill::get_border_size() const
{
    return border.size();
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
