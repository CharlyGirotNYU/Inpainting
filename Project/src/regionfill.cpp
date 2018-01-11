#include "regionfill.hpp"

RegionFill::RegionFill()
{
    patch_size=9; //has to be impaired
}


/** get and fill the border */
void RegionFill::fill_border()
{
    for(int i=0; i<im->alpha().rows; ++i)
    {
        for(int j=0; j<im->alpha().cols; ++j)
        {
            if(im->alpha(i,j) == 2)
            {
                border_point point;
                point.data_term = 0.0f;
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
    else if(nb_out == 8)
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


void RegionFill::compute_isophotes(float alpha)
{
    cv::Mat I;
    cvtColor( im->image(), I, CV_RGB2GRAY );

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;
    int ddepthC1 = CV_32SC1;

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

    isophotes_data_magnitude = magnitude;
    isophotes_data_orientation = orientation;
}

void RegionFill::init_confidence()
{
    double min, max;
    cv::minMaxLoc(im->mask(), &min, &max);
    confidence = (im->mask()==0)/255;
}

float RegionFill::compute_confidence(cv::Point2i p)
{
    cv::Mat patch = cv::Mat::zeros(patch_size,patch_size,im->image().type());

    int step = floor(patch_size/2);
    float conf=0.0f;

    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            patch.at<cv::Vec3b>(i+step,j+step) = im->get_image(i+p.y,j+p.x); //why just im->image(i,j) doesn't work ?? we changed the prototype in image.hc

            if(im->get_alpha(i+p.y,j+p.x) ==  SOURCE)
                conf += confidence.at<uchar>(i+p.y,j+p.x);
        }
    conf = conf/(patch_size*patch_size);

    return conf;
}

void RegionFill::compute_data_term()
{

    for(border_point point : border)
    {
        cv::Point p(point.coord.x,point.coord.y);


        cv::Mat gauss_x = cv::getGaussianKernel(3, 1 , CV_32F);
        cv::Mat gauss_y = cv::getGaussianKernel(3, 1, CV_32F);

        cv::Mat p_neighbors = cv::Mat::zeros(3,3,gauss_x.type());
        int step = floor(3/2);

        cv::Mat gauss = gauss_x*gauss_y.t();
        int size_x =  im->get_cols();
        int size_y =  im->get_rows();
        //std::cout << "size : " << size_x  <<" " << size_y;

        for(int i=-step; i<=step; ++i)
            for(int j=-step; j<=step; ++j)
            {

                if(((i+p.y)>0 && (i+p.y)<size_y)  && ((j+p.x)>0 && (j+p.x)<size_x) )
                {
                    if(im->alpha(i+p.y,j+p.x) == BORDER)
                    {
                        std::cout << "alpha : " << (int)im->alpha(i+p.y,j+p.x)  <<" ";
                        std::cout << "gauss : " << (float)gauss.at<float>(i+step,j+step)  << std::endl;
                        p_neighbors.at<float>(i+step,j+step) = gauss.at<float>(i+step,j+step);//1;
                    }
                }
            }


        //                cv::Point2f n_p =  compute_vector_normal(p,p_neighbors);
        //Verification de la direction de la normale
        //                int x,y;
        //                if(n_p.x < 0 )
        //                    x = (int)(floor(n_p.x)) + p.x;
        //                else
        //                    x = (int)(floor(n_p.x)) + p.x;
        //                if(n_p.y < 0 )
        //                    y = (int)(ceil(n_p.y)) + p.y;
        //                else
        //                    y = (int)(ceil(n_p.y)) + p.y;

        //                if(im->alpha(x,y) == SOURCE || im->alpha(x,y)== UPDATED)
        //                {
        //                    std::cout<< "Alpha " << (int)im->alpha(x,y) << std::endl;
        //                    std::cout<< "/*n_p*/.x" << x << "n_p.y" << y<< std::endl;
        //                    n_p = -n_p;
        //                }
        //                else if(im->alpha(x,y) == BORDER)
        //                    std::cout<<"Erreur dans la direction de la normale"<<std::endl;

        //                float mag = isophotes_data_magnitude.at<float>(p.x,p.y);
        //                float orien = isophotes_data_orientation.at<float>(p.x,p.y);
        //                cv::Point Ip(mag*cos(orien), mag*sin(orien));

        //                point.data_term = Ip.x * n_p.y - Ip.y * n_p.x;
        //                std::cout<<"Data term : " << point.data_term<<std::endl;

    }

}

/** Compute the vector n_p from the point p and his nieghbors */
cv::Point2f RegionFill::compute_vector_normal(cv::Point p,  cv::Mat p_neighbors)
{
    for(int i=0; i<3; ++i)
    {
        for(int j=0; j<3; ++j)
        {
            std::cout<<" " << (float)p_neighbors.at<float>(i,j) << " ";

        }
        std::cout<<std::endl;
    }


    p_neighbors.at<float>(1,1) = 0.0f;
    cv::Point max_loc1,max_loc2;
    double max;

    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc1);
    cv::Point previous = max_loc1;
    //        std::cout << "Max " << max << " Min " << max_loc;
    p_neighbors.at<float>(max_loc1) = 0.0f;
    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc2);
    //        std::cout << "Max " << max << " Min " << max_loc;
    cv::Point successive = max_loc2;


    //  Equation de la droite passant par le point successif et précédent forme ax+b
    float a = (successive.y - previous.y) / (successive.x - previous.x);
    float b = successive.y - a * successive.x;

    //    std::cout << "a " << a << " b " << b<<std::endl;
    //Equation de la normale y = n_a * x + n_b
    float n_a = -b;
    float n_b = a;
    //Calcul d'un deuxième point apparenant à la normale
    cv::Point point_n;
    point_n.x = 1.0;
    point_n.y = n_a * point_n.x + n_b;
    //    std::cout << "point_n.x " << point_n.x << " point_n.y " << point_n.y<<std::endl;
    //Calcul du vecteur normale
    cv:: Point2f n_p = p - point_n;
    //    std::cout << "n_p.x " << n_p.x << " n_p.y " << n_p.y<<std::endl;
    //Normalisation du vecteur
    float norm = cv::norm(n_p);
    std::cout<< "norm : "<< norm <<std::endl;
    n_p.x = (float)n_p.x / norm;
    n_p.y = (float)n_p.y / norm;
    //    std::cout << "n_p.x " << n_p.x << " n_p.y " << n_p.y<<std::endl;


    return n_p;
}





