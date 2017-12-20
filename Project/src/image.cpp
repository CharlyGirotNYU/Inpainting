#include "image.hpp"

Image::Image()
{
}



void Image::imread(std::string image_path, std::string mask_path)
{
    cv::Mat image = cv::imread(image_path, CV_LOAD_IMAGE_COLOR);
    cv::Mat mask  = cv::imread(mask_path, CV_LOAD_IMAGE_GRAYSCALE);

    int const Nu = image.cols;
    int const Nv = image.rows;

    //Init mat alpha
    alpha_data.zeros(Nv,Nu,CV_8UC1);
    for(int i=0; i<Nu; ++i)
        for(int j=0; j<Nv; ++j)
        {

        }
}


void Image::imwrite(std::string filepath)
{

}


cv::Mat const& Image::image() const
{
    return image_data;
}

cv::Mat const& Image::alpha() const
{
    return alpha_data;
}

int const& Image::image(int u, int v) const
{
    return image_data.at<int>(u,v);
}

int const& Image::alpha(int u, int v) const
{
    return alpha_data.at<int>(u,v);
}

int& Image::image(int u, int v)
{
    return alpha_data.at<int>(u,v);
}

int& Image::alpha(int u, int v)
{
    return alpha_data.at<int>(u,v);
}


bool Image::is_border(int u, int v)
{
    int nb_out = 0;
    for(int i=-1; i<=1 ; ++i)
        for(int j=-1; j<=1; ++j)
        {
            if(image(u+i,v+j))
            {
                return true;
            }
        }
    return false;
}
