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
//    patch_data = cv::Mat::zeros(patch_size,patch_size,im->image().type());
//    int step = floor(patch_size/2);

//    for(int i=-step; i<=step; ++i)
//        for(int j=-step; j<=step; ++j)
//        {
//            patch_data.at<cv::Vec3b>(i+step,j+step) = im->get_image_pixel(i+center.x,j+center.y);
//            if(source)
//                if(! (im->get_alpha_pixel(i+center.x,j+center.y) == SOURCE))
//                {
//                    patch_data.at<cv::Vec3b>(i+step,j+step) = cv::Vec3b(-1,-1,-1);
//                }

//        }

    patch_data = cv::Mat(im->image(),cv::Rect(center.x,center.y,patch_size,patch_size));
//    int step = floor(patch_size/2);
//    int x0=0,y0=0,x1=0,y1=0;
//    if(p.x-step < 0)                { x0=0; }                 else { x0= p.x-step; }
//    if(p.x+step >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= p.x+step; }
//    if(p.y-step < 0)                { y0=0; }                 else { y0= p.y-step; }
//    if(p.y+step >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= p.y+step; }
//    std::cout << "x0 x1 y0 y1 :" << x0 << " "<< x1 << " " << y0 << " " << y1 << std::endl;
//    std::cout << im->get_rows() << " " << im->get_cols() << std::endl;
//    Q(cv::Range(0,x1-x0),cv::Range(0,y1-y0)).copyTo(im->image()(cv::Range(x0,x1),cv::Range(y0,y1)));
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
            patch_data.at<cv::Vec3b>(i+step,j+step) = image_data->get_image_pixel(i+center.x,j+center.y);
            cv::Vec3b a = image_data->get_image_pixel(i+center.x,j+center.y);
            if(source)
                if(! image_data->get_alpha_pixel(i+center.x,j+center.y) == SOURCE)
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
            if(! image_data->get_alpha_pixel(i+center_data.x,j+center_data.y) == SOURCE)
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

//    for(int i=-step; i<=step; ++i)
//        for(int j=-step; j<=step; ++j)
//        {
//            if(a.get_pixel(i,j) == cv::Vec3b(-1,-1,-1))
//                patch_data.at<cv::Vec3b>(i,j) = cv::Vec3b(-1,-1,-1);
//        }

    int step = floor(patch_size/2);
    int x0=0,y0=0,x1=0,y1=0;
    if(a.center_data.x-step < 0)                { x0=0; }                 else { x0= a.center_data.x-step; }
    if(a.center_data.x+step >= image_data->get_rows())  { x1= image_data->get_rows()-1; } else { x1= a.center_data.x+step; }
    if(a.center_data.y-step < 0)                { y0=0; }                 else { y0= a.center_data.y-step; }
    if(a.center_data.y+step >= image_data->get_cols())  { y1= image_data->get_cols()-1; } else { y1= a.center_data.y+step; }
//    std::cout << "x0 x1 y0 y1 :" << x0 << " "<< x1 << " " << y0 << " " << y1 << std::endl;
//    cv::Mat tmp
     cv::Mat tmp = cv::Mat(image_data->image(),cv::Rect(x0,y0,x1-x0,y1-y0));
//    tmp(cv::Range(0,x1-x0),cv::Range(0,y1-y0)).copyTo(patch_data(cv::Range(0,x1-x0),cv::Range(0,y1-y0)),a.patch_data(cv::Range(0,x1-x0),cv::Range(0,y1-y0)));
}
