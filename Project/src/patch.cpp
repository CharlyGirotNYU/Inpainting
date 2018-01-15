#include "patch.hpp"
#include "image.hpp"

patch::patch()
{   
}


patch::patch(Image* im, int size, cv::Point2i center, bool source)
{
    image_data = im;
    patch_size = size;
    center_data = center;
    patch_data = cv::Mat::zeros(patch_size,patch_size,im->image().type());
    int step = floor(patch_size/2);

    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            patch_data.at<cv::Vec3b>(i+step,j+step) = im->get_image(i+center.x,j+center.y);
            if(source)
                if(! (im->get_alpha(i+center.x,j+center.y) == SOURCE))
                {
                    patch_data.at<cv::Vec3b>(i+step,j+step) = cv::Vec3b(-1,-1,-1);
                }

        }
}

patch::patch(Image* im, int size)
{
    image_data = im;
    patch_size = size;
    patch_data = cv::Mat::zeros(patch_size,patch_size,im->image().type());
    image_data = im;


}

void patch::set_center_and_fill(cv::Point2i center, bool source)
{

    int step = floor(patch_size/2);
//    std::cout << center.x << " "<< center.y << std::endl;
    assert(center.x >= step && center.y >= step);
    center_data = center;
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            patch_data.at<cv::Vec3b>(i+step,j+step) = image_data->get_image(i+center.x,j+center.y);
            cv::Vec3b a = image_data->get_image(i+center.x,j+center.y);
            if(source)
                if(! image_data->get_alpha(i+center.x,j+center.y) == SOURCE)
                {
                    patch_data.at<cv::Vec3b>(i+step,j+step) = cv::Vec3b(-1,-1,-1);
                }
        }
}

bool patch::is_whole_patch_source()
{
    int step = floor(patch_size/2);

    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            if(! image_data->get_alpha(i+center_data.x,j+center_data.y) == SOURCE)
                return false;
        }
    return true;
}

float patch::compute_distance_SSD_LAB(patch B)
{

    float dist = 0.0f;
    int step = floor(patch_size/2);
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
           // Get pixel (i,j) of both patch
            cv::Mat3b pixP (this->patch_data.at<cv::Vec3b>(i+step,j+step));
            cv::Mat3b pixQ (B.patch_data.at<cv::Vec3b>(i+step,j+step));
//            std::cout << (int)pixP.at<uchar>(0,0) << " " << (int)pixP.at<uchar>(0,1) << " "<< (int)pixP.at<uchar>(0,2) << std::endl;
//            std::cout << (int)pixQ[0][0][1] << std::endl;
            //Convert them to CIE Lab (L,a,b)
            cv::Mat3b pixPLab, pixQLab;
            cvtColor(pixP,pixPLab,cv::COLOR_RGB2Lab);
            cvtColor(pixQ,pixQLab,cv::COLOR_RGB2Lab);
            //Compute distance //Formula for CIE 76 // Update to CIE 94 (look wikipedia) if not working
            float dL = pixPLab.at<uchar>(0,0) - pixQLab.at<uchar>(0,0);
            float da = pixPLab.at<uchar>(0,1) - pixQLab.at<uchar>(0,1);
            float db = pixPLab.at<uchar>(0,2) - pixQLab.at<uchar>(0,2);
//            std::cout << pixPLab << std::endl;
//            std::cout << "dL " << dL << " da " << da << " db " << db <<std::endl;
            dist += std::sqrt(dL*dL + da*da + db*db);
        }
//    std::cout << dist << std::endl;
    return dist;

}

void patch::mask(patch a)
{
    int step = floor(patch_size/2);
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            if(a.get_pixel(i,j) == cv::Vec3b(-1,-1,-1))
                patch_data.at<cv::Vec3b>(i,j) = cv::Vec3b(-1,-1,-1);
        }
}
