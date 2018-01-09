#include "regionfill.hpp"

RegionFill::RegionFill()
{
}


/** get and fill the border */
void RegionFill::fill_border()
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
void RegionFill::update_border()
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
    int ddepth = CV_16S;

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
    cv::Sobel( I/max, grad_x, CV_32F, 1, 0, 3,scale, delta, cv::BORDER_DEFAULT);
    cv::Sobel( I/max, grad_y, CV_32F, 0, 1, 3,scale, delta, cv::BORDER_DEFAULT);
    cv::Mat orientation(im->get_rows(), im->get_cols(), CV_32FC1);
    cv::Mat magnitude(im->get_rows(), im->get_cols(), CV_32FC1);
    for(int i = 0; i < grad_y.rows; ++i)
        for(int j= 0; j< grad_y.cols; ++j)
        {
            orientation.at<float>(i,j) = atan2(grad_y.at<float>(i,j),grad_x.at<float>(i,j) ) * 180/M_PI ;
            magnitude.at<float>(i,j)= sqrt(grad_x.at<float>(i,j)*grad_x.at<float>(i,j)+grad_y.at<float>(i,j)*grad_y.at<float>(i,j));
        }

    cv::imshow(window_name,magnitude);
    cv::waitKey(0);

    isophotes_data_magnitude = magnitude;
    isophotes_data_orientation = orientation;
}
