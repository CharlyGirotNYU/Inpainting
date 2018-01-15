#include "regionfill.hpp"
#include <iomanip>

RegionFill::RegionFill()
{
    patch_size=25; //has to be impaired
}


/** get and fill the border */
void RegionFill::init_border()
{
//    cv::imshow("alpĥa " ,im->alpha()*100);
//    cv::waitKey(0);
    for(int i=0; i<im->alpha().cols; ++i)
    {
        for(int j=0; j<im->alpha().rows; ++j)
        {
            if(im->alpha(i,j) == BORDER)
            {
//                std::cout <<(int) im->alpha(i,j) << " i " << i << " j " << j << std::endl;
                border_point point;
                point.coord = cv::Point2i(i,j);
                point.data_term = 0.0f;
                point.priority = 0.0f;
                border.push_back(point);
            }
        }
    }
}

/** Update alpha  after a patch copy centered at bp */
void RegionFill::update_alpha()
{
    int step = floor(patch_size/2);
    for(border_point& bp : border)
    {
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
                    //update confidence
                    confidence = compute_confidence(bp.coord);
                }
            }

        // Update the possible border around the new patch. Only the points that
        //were in the mask can become a border (in the mask like not in the patch ? )

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
}

/** Return true if the point is a new border, else false */
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

void RegionFill::compute_isophotes()
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


float RegionFill::compute_data_term(cv::Point p)
{

    //Need to change y and x because of at in im->alpha(i,j)
    if(im->alpha(p.y,p.x) == BORDER)
    {
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
                        //                    std::cout << "alpha : " << (int)im->alpha(i+p.y,j+p.x)  <<" ";
                        //                    std::cout << "gauss : " << (float)gauss.at<float>(i+step,j+step)  << std::endl;
                        p_neighbors.at<float>(i+step,j+step) = gauss.at<float>(i+step,j+step);//1;
                    }
                }
            }

        cv::Point2f n_p =  compute_vector_normal(p,p_neighbors);
        // Verification de la direction de la normale
        int x,y;

        x = (int)(round(n_p.x)) + p.x;
        y = (int)(round(n_p.y)) + p.y;

        if(y>0 && y<size_y  && x>0 && x<size_x)
        {
            if(im->alpha(y,x) == SOURCE || im->alpha(y,x)== UPDATED)
            {
                //                std::cout<<"Source ou UPDATED"<<std::endl;
                n_p = -n_p;
            }
            else if(im->alpha(x,y) == BORDER)
                std::cout<<"Erreur dans la direction de la normale"<<std::endl;
            else ;
        }


        //std::cout << std::setprecision(3)<< "n_p = [" << n_p.x <<","<< n_p.y<<"];" <<std::endl;


        float mag = isophotes_data_magnitude.at<float>(p.y,p.x);
        float orien = isophotes_data_orientation.at<float>(p.y,p.x);
        cv::Point2f Ip(mag*cos(orien), mag*sin(orien));

//        std::cout << "Magnitude : "<<mag<<std::endl;
//        std::cout <<"Orientation : "<< orien << std::endl;


//        std::cout << "Ip = [" << Ip.x <<","<< Ip.y<<"];" <<std::endl;


        float data_term = Ip.x * n_p.y - Ip.y * n_p.x;
//        std::cout<<"Data term : " << data_term<<std::endl;

    }

    //float T = (successive.y - previous.y) / (successive.x - previous.x);

    return 0.1f; //debug
}

void RegionFill::compute_priority()
{
    for(border_point& bp : border)
        bp.priority = compute_confidence(bp.coord)*compute_data_term(bp.coord);
}

/** Running through patches along the border
* Maybe we shall return a border point and not a cv::Point2i */
cv::Point2i RegionFill::running_through_patches()
{
    float priority = 0.0f;
    cv::Point2i coord_priority;
    for( border_point& bPoint : border)
    {
        if(bPoint.priority > priority)
            coord_priority = bPoint.coord;
    }
    return coord_priority;
}

/** Check if the whole image has been processed
 * Takes a lot of time , mabe use iterator to gain efficiency
 * */
bool RegionFill::whole_image_processed()
{
    if(get_border_size() == 0)
        return true;
    return false;
}


/** find exemplar patch :
 * We search in the source region for the patch which is most similar to Pp.
 * d(Pp,Pq) is the sum of squaerd differences SSD of the ALREADY filled pixels in the two patches
 * We use the CIE Lab colour space because of its property of perceptual information
 * Receive : center point of the reference patch P
 * Return : the center point of the exemplar patch Q
 * */
//TODO : CHECK THAT THE WHOLE TESTED PATCH IS IN THE SOURCE
//todo : CHECK ALREADY FILLED PIXELS
cv::Point2i RegionFill::find_exemplar_patch(cv::Point2i p)
{
    cv::Mat patchP,patchQ;
    patchP= cv::Mat::zeros(patch_size,patch_size,im->image().type());
    patchQ =cv::Mat::zeros(patch_size,patch_size,im->image().type());
    //    patchP= cv::Mat::zeros(patch_size,patch_size,CV_32F);
    //    patchQ =cv::Mat::zeros(patch_size,patch_size,CV_32F);
    cv::Point2i coord_center_patchQ;

    //    float distance=0.0f;
    float distance_max = 10e-7;

    int step = floor(patch_size/2);


    //Store patchP //only pixels in the source
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            if(im->get_alpha(i+p.y,j+p.x) == SOURCE)
                patchP.at<cv::Vec3b>(i+step,j+step) = im->get_image(i+p.y,j+p.x);
            else
            {
                cv::Vec3b n; n.val[0]=0; n.val[1] = 0; n.val[2] = 0;
                patchP.at<cv::Vec3b>(i+step,j+step) = cv::Vec3b(0,0,0);
            }

        }

    //Run through all patch of the image
    //Each patch tested must be completely in the source
    //Do we test only pixels of the patch corresponding to the source's pixels that were in the source ?

    for(int u=step; u < im->get_rows() -step; ++u)
        for(int v=step; v < im->get_cols() - step; ++v)
        {


            if( u!=p.y && v!=p.x) // if the exemplar patch is not the current patch
            {
                bool compute_distance = true;
                //Store patchQ (temporarly) // if all the points are in the source
                for(int i=-step; i<=step; ++i)
                    for(int j=-step; j<=step; ++j)
                    {
                        if(im->get_alpha(u+i+step,j+v+step) == SOURCE)//all points in the source
                        {
                            if(im->get_alpha(i+p.y,j+p.x) == SOURCE) //only points corresponding to points of the patch tested which are also in the source (as not in the mask)
                                patchQ.at<cv::Vec3b>(i+step,j+step) = im->get_image(u+i+step,v+j+step);
                        }
                        else
                        {
                            patchQ =cv::Mat::zeros(patch_size,patch_size,im->image().type());
                            compute_distance = false; //This patch can't be used to propagate texture cause is not full
                            break;
                        }

                    }
                if(compute_distance)
                {
                    float distance = compute_patch_SSD_LAB(patchP,patchQ);

                    if(distance < 1/distance_max && distance != 0) //minimise la distance
                    {
                        distance_max = 1/distance;
                        coord_center_patchQ = cv::Point2i(u,v);
                        //std::cout << distance << std::endl;
                    }

                }

            }


        }





    return coord_center_patchQ;

    //    for(int u=step; u < im->get_rows() -step; ++u)
    //        for(int v=step; v < im->get_cols() - step; ++v)
    //        {
    //            distance = 0.0f;
    //            if(u != p.x && v != p.y)
    //            {
    //                //compute SSD of the two patches
    //                for(int i=-step; i<=step; ++i)
    //                    for(int j=-step; j<=step; ++j)
    //                    {
    //                        //Get pixel (i,j) of both patch
    //                        cv::Mat3b pixP (patchP.at<cv::Vec3b>(i+step,j+step));
    //                        cv::Mat3b pixQ (im->get_image(u+i+step,v+j+step));

    //                        //Convert them to CIE Lab (L,a,b)
    //                        cv::Mat3b pixPLab, pixQLab;
    //                        cvtColor(pixP,pixPLab,cv::COLOR_RGB2Lab);
    //                        cvtColor(pixQ,pixQLab,cv::COLOR_RGB2Lab);
    //                        //Compute distance //Formula for CIE 76 // Update to CIE 94 (look wikipedia) if not working
    //                        float dL = pixPLab[0][0][0] - pixQLab[0][0][0];
    //                        float da = pixPLab[0][0][1] - pixQLab[0][0][1];
    //                        float db = pixPLab[0][0][2] - pixQLab[0][0][2];
    //                        distance += cv::sqrt(dL*dL + da*da + db*db);
    //                    }


    //                    if(distance < 1/distance_max && distance != 0) //minimise la distance
    //                    {
    //                        distance_max = 1/distance;
    //                        coord_center_patchQ = cv::Point2i(u,v);
    //                        std::cout << distance << std::endl;
    //                    }

    //            }

    //        }
    //    return coord_center_patchQ;
}

float RegionFill::compute_patch_SSD_LAB(cv::Mat A,cv::Mat B)
{
    float dist = 0.0f;
    int step = floor(patch_size/2);
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            //Get pixel (i,j) of both patch
            cv::Mat3b pixP (A.at<cv::Vec3b>(i+step,j+step));
            cv::Mat3b pixQ (B.at<cv::Vec3b>(i+step,j+step));
            //            cv::Mat3b pixQ (im->get_image(u+i+step,v+j+step));

            //Convert them to CIE Lab (L,a,b)
            cv::Mat3b pixPLab, pixQLab;
            cvtColor(pixP,pixPLab,cv::COLOR_RGB2Lab);
            cvtColor(pixQ,pixQLab,cv::COLOR_RGB2Lab);
            //Compute distance //Formula for CIE 76 // Update to CIE 94 (look wikipedia) if not working
            float dL = pixPLab[0][0][0] - pixQLab[0][0][0];
            float da = pixPLab[0][0][1] - pixQLab[0][0][1];
            float db = pixPLab[0][0][2] - pixQLab[0][0][2];
            dist += cv::sqrt(dL*dL + da*da + db*db);
        }
    return dist;
}

void RegionFill::propagate_texture(cv::Point2i p, cv::Point2i q)
{

    int step = floor(patch_size/2);
    for(int i=-step; i<=step; ++i)
        for(int j=-step; j<=step; ++j)
        {
            if(im->get_alpha(i+p.x+step,j+p.y+step) != SOURCE) // DO NOT replace pixels from the source
                im->image(i+p.x+step,j+p.y+step) = im->image(i+q.x+step,j+q.y+step);
        }
}

/** Perform the algorithm needed to copy texture
 * */
void RegionFill::run()
{

    //verif que l'image n'est pas nulle à faire avant de pouvoir lancer l'algo ...

    //init confidence
    init_confidence();
    //Compute priorities of the border points
    init_border();
    // While there are IN in alpha (all the mask hasn't been updated)
//    while(!whole_image_processed())
    {
        /** 1.a */
        //Done with init_border then next by update alpha which update border stored in "border"
        /** 1.b */
        compute_isophotes(); //Compute isophotes
        compute_priority();
        /** 2.a */
        cv::Point2i point_priority = running_through_patches();
        /** 2.b */
        //find_exemplar_patch (minimizing d(Pp,Pq)
        cv::Point2i point_exemplar = find_exemplar_patch(point_priority);
//        std::cout <<"point priority " << point_priority << std::endl;
//        std::cout <<"point exemplar " << point_exemplar << std::endl;
        /** 2.c */
        //propagate_texture
        propagate_texture(point_priority, point_exemplar);
        /** 3 */
        update_alpha(); //Actually : update alpha, border, confidence
    }
    im->imwrite("result.png");
}

/** Compute the vector n_p from the point p and his nieghbors */
cv::Point2f RegionFill::compute_vector_normal(cv::Point p,  cv::Mat p_neighbors)
{

    p_neighbors.at<float>(1,1) = -1.0f;
    cv::Point max_loc1,max_loc2;
    double max;

    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc1);
    cv::Point pre = max_loc1;
    //        std::cout << "Max " << max << " Min " << max_loc;
    p_neighbors.at<float>(max_loc1) = -1.0f;
    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc2);
    //        std::cout << "Max " << max << " Min " << max_loc;
    cv::Point suc = max_loc2;
    p_neighbors.at<float>(max_loc2) = -1.0f;


    //Compute the middle of line between previous and successive
    cv::Point2f milieu;
    milieu.x  = (suc.x+pre.x)/2.0f-1.0f;
    milieu.y = (suc.y+pre.y)/2.0f-1.0f;
    //Compute the distance between the middle and p
    float distance = sqrt((milieu.x)*(milieu.x)+(milieu.y)*(milieu.y));

    cv::Point2f n_p;

    // If the distance between p and the middle is close to 0 => horizontal case or vertical case
    if(distance <= 0.1)
    {
        cv::Point point = p ;

        if(abs(suc.x-1)>0.5)
            point.y +=1 ;//If horizontal case
        else
            point.x +=1; //Vertical case
        n_p=p-point;
    }
    else
        n_p= milieu;

    //Normalisation du vecteur
    float norm = cv::norm(n_p);
    n_p.x = (float)n_p.x / norm;
    n_p.y = (float)n_p.y / norm;
    return n_p;
}

void RegionFill::test_compute_data_term()
{
    // Cas 0 : 187,101 // Cas 0 inverse : 190,122
    cv::Point cas_0(187,101);
    cv::Point cas_inv_0(190,122);

    // Cas 1 : 91, 117 // Cas 1 :inverse 69 112
    cv::Point cas_1(91,117);
    cv::Point cas_inv_1(69,112);

    // Cas 2 : 93 , 80  // Cas 2 inverse 70 115
    cv::Point cas_2(93,80);
    cv::Point cas_inv_2(70,115);
    // Cas 3 :  347 90 //  Cas 3 inverse 367 159
    cv::Point cas_3(347,90);
    cv::Point cas_inv_3(367,159);

    // Cas 4 : 351 ,  100 // Cas 4 inverse 171 101
    cv::Point cas_4(351,100);
    cv::Point cas_inv_4(171,101);

    // Cas 5 : 365 , 99   Cas 5 inverse 70,102
    cv::Point cas_5(365,99);
    cv::Point cas_inv_5(70,102);



    //    std::cout<<"Debut cas NORMAL"<<std::endl;
    compute_data_term(cas_0);
//    std::cout<<std::endl;
    //    std::cout<<"Debut cas INVERS"<<std::endl;
    compute_data_term(cas_inv_0);

}

