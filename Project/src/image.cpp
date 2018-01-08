#include "image.hpp"

Image::Image()
{

}



void Image::imread(std::string image_path, std::string mask_path)
{

    // READ IMAGE
    cv::Mat image = cv::imread(image_path, CV_LOAD_IMAGE_COLOR);
    image_data = image;
    if (!image.data) {
        std::cout << "Error reading file 1 "<< image_path << std::endl;
        exit(0);
    }
    // READ MASK
    cv::Mat mask  = cv::imread(mask_path, CV_LOAD_IMAGE_GRAYSCALE);
    mask_data = mask;
    if (!mask.data  ) {
        std::cout << "Error reading file 2" << mask_path << std::endl;
        exit(0);
    }

    //Set Image size
    Nu = image.cols;
    Nv = image.rows;

    //Init mat alpha IN, BORDER, SOURCE (Can't be UPDATED at this point)
    alpha_data = Mat::zeros(Nv, Nu, CV_8UC1);

    for(int i=1; i<Nu; ++i)
        for(int j=1; j<Nv; ++j)
        {
            int nb_out = num_outside_mask(i,j);

            if(nb_out == 0)
                alpha(i,j) = IN;
            else if(nb_out >0 && nb_out<8)
                alpha(i,j) = BORDER;
            else
                alpha(i,j) = SOURCE;

        }
}


void Image::imwrite(std::string filename)
{
    std::cout << image_data.cols << std::endl;
    cv::imwrite(filename,image_data);

    cv::imwrite("alpha.jpg", alpha_data*100); //*100 Just for visualizatio
}


cv::Mat const& Image::image() const
{
    return image_data;
}
cv::Mat const& Image::mask() const
{
    return mask_data;
}

cv::Mat const& Image::alpha() const
{
    return alpha_data;
}

int const& Image::image(int u, int v) const
{
    return image_data.at<int>(u,v);

}
int const& Image::mask(int u, int v) const
{
    return mask_data.at<int>(u,v);
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


int Image::num_outside_mask(int u, int v)
{
    int nb_out = 0;
    //Going through all the neighborhoods
    for(int i=-1; i<=1 ; ++i)
        for(int j=-1; j<=1; ++j)
        {
            if(u+i < Nu && v+j < Nv && u+i >0 && v+j >0) //gestion des bordures
                if(mask(u+i,v+j) != 0)
                    nb_out++;
        }
    return nb_out;
}
