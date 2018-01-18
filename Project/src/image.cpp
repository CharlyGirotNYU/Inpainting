#include "image.hpp"

static std::string type2str(int type);

Image::Image()
{}

void Image::imread(std::string image_path, std::string mask_path)
{

    // READ IMAGE
    cv::Mat image_read = cv::imread(image_path,CV_LOAD_IMAGE_UNCHANGED);
    image_data = image_read;
    if (!image_read.data) {
        std::cout << "Error reading file 1 "<< image_path << std::endl;
        exit(0);
    }
    // READ MASK


    cv::Mat mask_read  = cv::imread(mask_path);
    cvtColor( mask_read, mask_data, CV_RGB2GRAY );
    if (!mask_read.data  ) {
        std::cout << "Error reading file 2" << mask_path << std::endl;
        exit(0);
    }

    //Set Image size
    Nu = image_read.rows;
    Nv = image_read.cols;

    //Init & Set mat alpha IN, BORDER, SOURCE (Can't be UPDATED at this point)
    alpha_data = Mat::zeros(Nu, Nv, CV_8UC1);
    for(int j=0; j<Nv; ++j) //colonne
        for(int i=0; i<Nu; ++i) //ligne
        {
            int nb_out = num_outside_mask(i,j);

            if(nb_out == 0)
                set_alpha_pixel(i,j) = SOURCE;
            else if(nb_out >0 && nb_out<8)
                set_alpha_pixel(i,j) = get_mask_pixel(i,j)!=0 ? BORDER : SOURCE;
            else
                set_alpha_pixel(i,j) = IN;
        }
//    cv::imshow("alpha dans image.cpp" , alpha()*100);
//    cv::waitKey(0);
}


void Image::imwrite(std::string filename)
{
    cv::imwrite(filename,image_data);
    cv::imwrite("alpha.bmp", alpha_data*50); //*100 Just for visualizatio
}

/** Get Image mat */
cv::Mat Image::image() const
{
    return image_data;
}
cv::Mat& Image::image() {return image_data;}

/** Get mask mat */
cv::Mat Image::mask() const
{
    return mask_data;
}
/** Get alpha mat */
cv::Mat Image::alpha() const
{
    return alpha_data;
}

/** Get image_data pixel */
cv::Vec3b Image::get_image_pixel(int u, int v) const
{
//    std::cout << "get "<< std::endl;
    return image_data.at<cv::Vec3b>(u,v);
}

/** Get mask pixel*/
uchar Image::get_mask_pixel(int u, int v) const
{
    return mask_data.at<uchar>(u,v);
}
/** Get alpha pixel */
uchar Image::get_alpha_pixel(int u, int v) const
{
//    std::cout << "get "<< std::endl;
    return alpha_data.at<uchar>(u,v);
}
/** Set image pixel */
cv::Vec3b &Image::set_image_pixel(int u, int v)
{
//    std::cout << "set"<<std::endl;
    return image_data.at<cv::Vec3b>(u,v);
}
/** Set alpha pixel */
uchar &Image::set_alpha_pixel(int u, int v)
{
//    std::cout << "set"<<std::endl;
    return alpha_data.at<uchar>(u,v);
}


const int& Image::get_rows() const
{
    return Nu;
}

const int& Image::get_cols() const
{
    return Nv;
}


int Image::num_outside_mask(int u, int v)
{
    int nb_out = 0;
    //Going through all the neighborhoods
    for(int j=-1; j<=1; j++)
        for(int i=-1; i<=1 ; i++)
        {
            if(!(i==0 && j == 0)) //don't test the center pixel
                if(u+i>=0 && u+i<Nu) //Check if within the image region/size
                    if( v+j<Nv && v+j>=0)
                        if(get_mask_pixel(u+i,v+j) != 0)
                            nb_out++;
        }
    return nb_out;
}


std::string type2str(int type) {
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
    }

    r += "C";
    r += (chans+'0');

    return r;

    //HOW TO USE
    //Display Type of the matric // FOR DEBUG
    //    std::string ty =  type2str( mask_data.type() );
    //    printf("Matrix: %s %dx%d \n", ty.c_str(), Nv, Nu );
}
