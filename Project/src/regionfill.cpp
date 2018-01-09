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
    std::cout << im->get_cols() << std::endl;
    isophotes_data = cv::Mat::zeros(im->get_rows(),im->get_cols(), CV_8UC2);
    cv::Mat I = im->image();// / 255;
    cvtColor( im->image(), I, CV_RGB2GRAY );
    cv::Mat theta = cv::Mat::zeros(im->get_rows(),im->get_cols(), CV_8UC1);

    cv::Mat grad_x, grad_y;
    cv::Mat abs_grad_x, abs_grad_y;
    cv::Mat grad;

    char* window_name = "Sobel Demo - Simple Edge Detector";
      int scale = 1;
      int delta = 0;
      int ddepth = CV_16S;

    /// Gradient X
    cv::Sobel( I, grad_x, ddepth, 1, 0, 3, scale, delta, cv::BORDER_DEFAULT );
    /// Gradient Y
    cv::Sobel( I, grad_y, ddepth, 0, 1, 3, scale, delta, cv::BORDER_DEFAULT );

    cv::convertScaleAbs( grad_x, abs_grad_x );
    cv::convertScaleAbs( grad_y, abs_grad_y );

    cv::Mat grad_x_p2, grad_y_p2, grad_p2;
    cv::pow(abs_grad_x,2, grad_x_p2);
    cv::pow(abs_grad_y,2, grad_y_p2);

    cv::addWeighted( grad_x_p2, 0.5, grad_y_p2, 0.5, 1, grad_p2 );

    cv::Mat newgradp2;
    grad_p2.convertTo(newgradp2, CV_64F);
//    grad.convertTo(grad,CV_64F);
    cv::pow(newgradp2,0.5,grad);
//    cv::sqrt(grad_p2,grad_p2);

    //cv::addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

    double min, max;
    cv::minMaxLoc(grad, &min, &max);

    cv::Mat norme_grad = grad/max;
    std::cout<< max << std::endl;
    cv::Mat T;
    std::cerr << norme_grad.type() << " " << norme_grad.channels() << " " << norme_grad.size() << std::endl;
    cv::threshold( norme_grad, T, max, 255,0 );

//    cv::imshow( window_name, T );
//     cv::waitKey(0);



    //MATLAB
//    %
//    % Usage:  [ I, theta ] = isophote( L, alpha );
//    %
//    % Argument:   L     -  Luminance of the input image [0 255]
//    %             alpha -  Threshold of isophotes       [0 1]
//    %
//    % Returns:    I     -  Magnitude of the isophotes   [0 1]
//    %             theta -  Angle of the isophotes   [-pi/2 +pi/2]

//    % Vahid. K. Alilou
//    % Department of Computer Engineering
//    % The University of Semnan
//    %
//    % December 2014

//    function [ I, theta ] = isophote( L, alpha )
//        L = double(L)/255; theta=zeros(size(L));
//        [Lx,Ly] = gradient(L);
//        I = sqrt(Lx.^2+Ly.^2);
//        I = I./max(max(I));
//        T = I>=alpha;
//        theta(T) = atan(Ly(T)./Lx(T));
//        I(I<alpha)=0;
//    end
}
