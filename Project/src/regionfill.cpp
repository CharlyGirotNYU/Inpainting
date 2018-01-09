#include "regionfill.hpp"

RegionFill::RegionFill()
{
    patch_size=51; //has to be impaired
}


/** get and fill the border */
void RegionFill::fill_border()
{
    for(int i=0; i<im->alpha().rows; ++i)
    {
        for(int j=0; j<im->alpha().cols; ++j)
        {
            if(im->alpha(i,j) == 1)
            {
                border_point point;
//                point.confidence = 0.f;
                point.coord = cv::Point2i(i,j);
                border.push_back(point);
            }
        }
    }
}

/** Update alpha  after a patch copy centered at bp */
void RegionFill::update_alpha(border_point bp)
{
    int step = floor(patch_size/2);
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
    int Nu = im->get_cols();
    int Nv= im->get_rows();

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
    return false; //TODO : Add for default but we need to Check
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
    //    cv::Mat theta = cv::Mat::zeros(im->get_rows(),im->get_cols(), CV_8UC1);

        cv::Mat I;// / 255;
        cvtColor( im->image(), I, CV_RGB2GRAY );
    //    I = I /255;


    //    cv::Mat grad_x, grad_y;
    //    cv::Mat abs_grad_x, abs_grad_y;
    //    cv::Mat grad;
    //    cv::Mat grad_x_p2, grad_y_p2, grad_p2;
    //    cv::Mat newgradp2;
    //    cv::Mat norme_grad;
    //    cv::Mat T;
    //    cv::Mat magnitude;

    std::string window_name = "Sobel Demo - Simple Edge Detector";
    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;
    int ddepthC1 = CV_32SC1;

    //    /// Gradient X
    //    cv::Sobel( I, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    //    /// Gradient Y
    //    cv::Sobel( I, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );

    //    cv::convertScaleAbs( grad_x, abs_grad_x );
    //    cv::convertScaleAbs( grad_y, abs_grad_y );

    //    cv::pow(abs_grad_x,2, grad_x_p2);
    //    cv::pow(abs_grad_y,2, grad_y_p2);

    //    cv::addWeighted( grad_x_p2, 0.5, grad_y_p2, 0.5, 1, grad_p2 );


    //    grad_p2.convertTo(newgradp2, CV_32F);
    //    cv::pow(newgradp2,0.5,grad);



    //    norme_grad = grad/max;

    //    //std::cerr << norme_grad.type() << " " << norme_grad.channels() << " " << norme_grad.size() << std::endl;
    //    cv::threshold( norme_grad, T,0.90, 255,0 );

    ////    cv::minMaxLoc(T, &min, &max);
    ////    std::cout << "Min T :" << max << std::endl;
    ////    cv::imshow( window_name, T );
    ////     cv::waitKey(0);

    //     cv::threshold( norme_grad, magnitude,0.90, 255,1 );

    //     cv::Mat mask = T>1;

    //     std::cerr << grad_y.type() << " " <<mask.type() <<std::endl;

    //     cv::Mat LyT,LxT;
    //     LyT.setTo(grad_y,mask);
    //     LxT.setTo(grad_x,mask);

    double min, max;
    cv::minMaxLoc(I, &min, &max);

    cv::Mat grad_x,grad_y;
//    cv::Sobel( I/max, grad_x, CV_32F, 1, 0, 3,scale, delta, cv::BORDER_DEFAULT);
//    cv::Sobel( I/max, grad_y, CV_32F, 0, 1, 3,scale, delta, cv::BORDER_DEFAULT);
    cv::Scharr(I/max,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
    cv::Scharr(I/max,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);
    cv::Mat orientation(im->get_rows(), im->get_cols(), ddepthC1);
    cv::Mat magnitude(im->get_rows(), im->get_cols(), ddepthC1);
    for(int i = 0; i < grad_y.rows; ++i)
        for(int j= 0; j< grad_y.cols; ++j)
        {
            orientation.at<float>(i,j) = atan2(grad_y.at<float>(i,j),grad_x.at<float>(i,j) ) * 180/M_PI ;
            magnitude.at<float>(i,j)= sqrt(grad_x.at<float>(i,j)*grad_x.at<float>(i,j)+grad_y.at<float>(i,j)*grad_y.at<float>(i,j));
        }

//    cv::imshow(window_name,magnitude);
//    cv::waitKey(0);

    isophotes_data_magnitude = magnitude;
    isophotes_data_orientation = orientation;
}

void RegionFill::init_confidence()
{
    confidence = (im->mask()==0);
//    cv::imshow("window_name",confidence);
//    cv::waitKey(0);
}

float RegionFill::compute_confidence(cv::Point2i p)
{
    cv::Mat patch = cv::Mat::zeros(patch_size,patch_size,im->image().type());

    int step = floor(patch_size/2);

    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            std::cout << "i " << i << " i + step " << i + step << std::endl;
            std::cout << "j " << j << " j + step " << j + step << std::endl;
            patch.at<cv::Vec3b>(i+step,j+step) = im->image(i+p.y,j+p.x);
            std::cout << (int)im->image(i+p.y,j+p.x) << std::endl;
        }
        cv::imshow("window_name",patch);
        cv::waitKey(0);
    //return 0.0f;

}
