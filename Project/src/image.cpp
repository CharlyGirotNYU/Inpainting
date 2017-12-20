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

    //Init mat alpha IN, BORDER, SOURCE (Can't be UPDATED at this point)
    alpha_data.zeros(Nv,Nu,CV_8UC1);
    cv::resize(alpha_data,alpha_data,image.size());
    std::cout << image.rows << std::endl;
    for(int i=1; i<Nu; ++i)
        for(int j=1; j<Nv; ++j)
        {
            int nb_out = num_outside_mask(i,j);
            std::cout << nb_out;
            if(nb_out == 0)
            {
                std::cout << i << std::endl;
                alpha(i,j) = 0;

            }
//            else if(nb_out >0 && nb_out<8)
//                alpha(i,j) = BORDER;
//            else
//                alpha(i,j) = SOURCE;
        }
}


void Image::imwrite(std::string filename)
{

    cv::imwrite(filename,image_data);
    cv::imwrite("alpha.png",alpha_data);
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
       alpha_data.at<int>(u,v);
}


int Image::num_outside_mask(int u, int v)
{
    //TODO : Gestion des bordures de l'image ...
//    int nb_out = 0;
//    for(int i=-1; i<=1 ; ++i)
//        for(int j=-1; j<=1; ++j)
//        {
//            if(image(u+i,v+j) != 0)
//            {
//                nb_out++;
//            }
//        }
    return 0;
}
