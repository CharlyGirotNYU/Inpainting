#include "patch.hpp"
#include "image.hpp"

patch::patch()
{   
}


patch::patch(Image* im, int x,int y, cv::Point2i center, bool source)
{
    //TODO : If source is true , else
    image_data = im;
    sizex = x;
    sizey = y;
    center_data = center;
    patch_data = cv::Mat::zeros(sizex,sizey,im->image().type());

    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    int x0=0,y0=0,x1=0,y1=0;
    if(center_data.x-stepx < 0)                { x0=0; }                 else { x0= center_data.x-stepx; }
    if(center_data.x+stepx >= image_data->get_rows())  { x1= image_data->get_rows()-1; } else { x1= center_data.x+stepx; }
    if(center_data.y-stepy < 0)                { y0=0; }                 else { y0= center_data.y-stepy; }
    if(center_data.y+stepy >= image_data->get_cols())  { y1= image_data->get_cols()-1; } else { y1= center_data.y+stepy; }

    sizex = x1-x0+1;
    sizey = y1-y0+1;

//    std::cout << center.x << " " << center.y << std::endl;
//    patch_data = cv::Mat(im->image(),cv::Rect(center.x-stepx,center.y-stepy,sizex,sizey));
    patch_data = image_data->image()(cv::Range(x0,x1),cv::Range(y0,y1));
}

patch::patch(Image* im, int x, int y)
{
    image_data = im;
    sizex = x;
    sizey = y;
    patch_data = cv::Mat::zeros(sizex,sizey,im->image().type());
}

bool patch::set_center_and_fill(cv::Point2i center, bool source)
{
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    if(center.x-stepx < 0 || center.x+stepx >= image_data->get_rows() || center.y-stepy < 0 || center.y+stepy >= image_data->get_cols())
        return false;

    center_data = center;
    patch_data = image_data->image()(cv::Range(center.x-stepx,center.x+stepx),cv::Range(center.y-stepy,center.y+stepy));

    return true;

      // Modification : ne pas modifier la taille du patch Q, plutôt invalider le patch si sa taille dépasse de l'image (ne devrait pas arriver)
//    int x0=0,y0=0,x1=0,y1=0;
//    if(center_data.x-stepx < 0)                { x0=0; }                 else { x0= center_data.x-stepx; }
//    if(center_data.x+stepx >= image_data->get_rows())  { x1= image_data->get_rows()-1; } else { x1= center_data.x+stepx; }
//    if(center_data.y-stepy < 0)                { y0=0; }                 else { y0= center_data.y-stepy; }
//    if(center_data.y+stepy >= image_data->get_cols())  { y1= image_data->get_cols()-1; } else { y1= center_data.y+stepy; }

//    sizex = x1-x0+1;
//    sizey = y1-y0+1;
}

bool patch::is_whole_patch_source()
{
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);
    for(int i=-stepx; i<=stepx; ++i)
        for(int j=-stepy; j<=stepy; ++j)
        {
            if(image_data->get_alpha_pixel(i+center_data.x,j+center_data.y) != SOURCE)
                return false;
        }
    return true;
}

float patch::compute_distance_SSD_LAB(patch B)
{
    float dist = 0.0f;
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);
    for(int i=0; i<sizex; ++i)
        for(int j=0; j<sizey; ++j)
        {
            if(this->get_alpha().at<uchar>(center_data.x-stepx+i,center_data.y-stepy+j) == SOURCE || this->get_alpha().at<uchar>(center_data.x-stepx+i,center_data.y-stepy+j) == UPDATED)
            {

                // Get pixel (i,j) of both patch
                cv::Mat3b pixP = cv::Mat3b::zeros(1,1);
                pixP = this->patch_data.at<cv::Vec3b>(i,j);
                cv::Mat3b pixQ = cv::Mat3b::zeros(1,1);
                pixQ = B.patch_data.at<cv::Vec3b>(i,j);
                //            std::cout << (int)pixP.at<uchar>(0,0) << " " << (int)pixP.at<uchar>(0,1) << " "<< (int)pixP.at<uchar>(0,2) << std::endl;
                //            std::cout << (int)pixQ[0][0][1] << std::endl;
    //            std::cout <<"RGB " << pixP << std::endl;
                //Convert them to CIE Lab (L,a,b)
                cv::Mat3b pixPLab = cv::Mat3b::zeros(1,1);
                cv::Mat3b pixQLab = cv::Mat3b::zeros(1,1);
                cvtColor(pixP,pixPLab,cv::COLOR_RGB2Lab);
                cvtColor(pixQ,pixQLab,cv::COLOR_RGB2Lab);
    //            std::cout <<"LAB " << pixPLab << std::endl;
    //            std::cout << "LAB 0 " << (int)pixPLab.at<uchar>(1) << std::endl;
                //Compute distance //Formula for CIE 76
                float dL = pixPLab.at<uchar>(0) - pixQLab.at<uchar>(0);
                float da = pixPLab.at<uchar>(1) - pixQLab.at<uchar>(1);
                float db = pixPLab.at<uchar>(2) - pixQLab.at<uchar>(2);

//                std::cout << "dL " << dL << " da " << da << " db " << db ;
                dist += (float)std::sqrt(dL*dL + da*da + db*db);
            }
        }
//       std::cout << dist << std::endl;
    return dist;
}

void patch::mask(patch P, bool src)
{
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

//    std::cout << "Fonction MASK " << std::endl;
//    std::cout << "step " << stepx << " " << stepy << std::endl;

      // Pas besoin de revérifier la taille de P : déja fait à l'initialisation, et P pas changé depuis
//    int x0=0,y0=0,x1=0,y1=0;
//    if(P.center_data.x-stepx < 0)                { x0=0; }                 else { x0= P.center_data.x-stepx; }
//    if(P.center_data.x+stepx >= image_data->get_rows())  { x1= image_data->get_rows()-1; } else { x1= P.center_data.x+stepx; }
//    if(P.center_data.y-stepy < 0)                { y0=0; }                 else { y0= P.center_data.y-stepy; }
//    if(P.center_data.y+stepy >= image_data->get_cols())  { y1= image_data->get_cols()-1; } else { y1= P.center_data.y+stepy; }

//    sizex = x1-x0 + 1;
//    sizey = y1-y0 + 1;
//    std::cout << "size apres verif : " << sizex << " " << sizey << std::endl;

    cv::Mat alpha_Q = cv::Mat::zeros(sizex,sizey,get_alpha().type());
    cv::Mat mask_a = cv::Mat::zeros(sizex,sizey,get_alpha().type());
    cv::Mat mask_alpha = P.get_alpha()(cv::Range(P.center_data.x-stepx,P.center_data.x+stepx),cv::Range(P.center_data.y-stepy,P.center_data.y+stepy));//,cv::Rect(x0,y0,sizex,sizey));

    if(src)
        mask_a = (mask_alpha == SOURCE);// + (mask_alpha == UPDATED);
    else
        mask_a = (mask_alpha == BORDER )+ (mask_alpha == IN); //if updated ?
    patch_data.copyTo(alpha_Q,mask_a);
    patch_data = alpha_Q;

    //    cv::imshow("alphaP", alpha_Q);
    //    cv::waitKey(0);
}
