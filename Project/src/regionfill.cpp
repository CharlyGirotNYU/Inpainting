#include "regionfill.hpp"
#include <iomanip>
#include "patch.hpp"

RegionFill::RegionFill()
{
    patch_size_x=9; //has to be impaired
    patch_size_y=9;
}


/** get and fill the border */
void RegionFill::init_border()
{
    //    cv::imshow("alpĥa " ,im->alpha()*100);
    //    cv::waitKey(0);

    for(int j=0; j<im->alpha().cols; ++j)
    {
        for(int i=0; i<im->alpha().rows; ++i)
        {
            if(im->get_alpha_pixel(i,j) == BORDER)
            {
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
void RegionFill::update_alpha(cv::Point2i bp, int sizex, int sizey)
{
    //TODO check
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);


    int x0=0,y0=0,x1=0,y1=0;
    if(bp.x-stepx < 0)                { x0=0; }                 else { x0= bp.x-stepx; }
    if(bp.x+stepx >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= bp.x+stepx; }
    if(bp.y-stepy < 0)                { y0=0; }                 else { y0= bp.y-stepy; }
    if(bp.y+stepy >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= bp.y+stepy; }
    std::cout << "x0 x1 y0 y1 :" << x0 << " "<< x1 << " " << y0 << " " << y1 << std::endl;

    // Update alpha of the points belonging to the patch arround the border point

    for(int j=y0; j<=y1; ++j)
        for(int i=x0; i<=x1; ++i)
        {
            //int x = bp.coord.x+i; int y = bp.coord.y+j;
            int x = bp.x+i; int y = bp.y +j;
            if(im->get_alpha_pixel(x,y) != SOURCE)
            {
                im->set_alpha_pixel(x,y) = UPDATED;
                border_point new_bp;
                new_bp.coord = cv::Point2i(x,y);
                new_bp.data_term=0.0f;
                new_bp.priority=0.0f;
                update_border(new_bp.coord,UPDATED); //PB
                //                    //update confidence
                //                    confidence = compute_confidence(bp.coord);
            }
        }

    // Update the possible border around the new patch. Only the points that
    //were in the mask can become a border (in the mask like not in the patch ? )

    //Taking care of the angles
    if(bp.x-stepx-1 < 0)                { x0=0; }                 else { x0= bp.x-stepx-1; }
    if(bp.x+stepx+1 >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= bp.x+stepx+1; }
    if(bp.y-stepy-1 < 0)                { y0=0; }                 else { y0= bp.y-stepy-1; }
    if(bp.y+stepy+1 >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= bp.y+stepy+1; }

    for(int i = x0; i<x1; ++i)
    {
        //ligne gauche
        int j=y0;
        if(is_new_border(i,j))
            update_border(cv::Point(i,j),BORDER);
        //ligne droite
        j=y1;
        if(is_new_border(i,j))
            update_border(cv::Point(i,j),BORDER);
    }
    for(int j = y0; j<y1; ++j)
    {
        //ligne haut
        int i=x0;
        if(is_new_border(i,j))
            update_border(cv::Point(i,j),BORDER);
        //ligne bas
        i=x1;
        if(is_new_border(i,j))
            update_border(cv::Point(i,j),BORDER);
    }
}

/** Return true if the point is a new border, else false */
bool RegionFill::is_new_border(int u, int v)
{
    int Nu = im->get_rows();
    int Nv= im->get_cols();

    int nb_out = 0;
    //Compute the number of points UPDATED or SOURCE around the neighborhoods
    for(int j=-1; j<=1; j++)
        for(int i=-1; i<=1 ; i++)
        {
            if(!(i==0 && j == 0)) //don't test the center pixel
                if(u+i>=0 && u+i<Nu) //Check if within the image region/size
                    if( v+j<Nv && v+j>=0)
                        if(im->get_alpha_pixel(u+i,v+j) == UPDATED || im->get_alpha_pixel(u+i,v+j) == SOURCE)
                            nb_out++;
        }

    if(nb_out >0 && nb_out<= 8)
        return im->get_alpha_pixel(u,v)==IN ? true : false;
    else return false;

    return false; //TODO : Add for default but we need to Check
}


/** Update the border according to the status :
        - IN = belongs to the mask - nothing to do
        - BORDER = need to be added to the vector
        - UPDATED = previously BORDER or IN, need to be removed from the vector
        - SRC = nothing to do (theses points never change)

    General use : Called during the alpha_data update when a point changes status*/
void RegionFill::update_border(cv::Point2i point, int status)
{
    border_point b;
    b.coord = point;
    if(status == BORDER)
    {
        border.push_back(b);
        //Update the alpha value
        im->set_alpha_pixel(b.coord.x,b.coord.y) = BORDER;
    }
    else if(status == UPDATED)
    {
        //Update the alpha value
        im->set_alpha_pixel(b.coord.x,b.coord.y) = UPDATED;

        int i=0;
        for(auto bp : border)
        {
            if(bp.coord == b.coord)
                border.erase(border.begin()+i);
            i++;
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

cv::Point RegionFill::compute_isophotes(cv::Point2i p)
{
    cv::Mat I;
    cvtColor( im->image(), I, CV_RGB2GRAY );

    int stepx = floor(patch_size_x/2);
    int stepy = floor(patch_size_y/2);

    cv::Mat I_patch(patch_size_x,patch_size_y,0);
    for(int j=-stepy; j<=stepy; ++j)
        for(int i=-stepx; i<=stepx; ++i)
        {
            if(p.x+i > 0 && p.x +i < im->get_rows() &&  p.y+j > 0 && p.y +j < im->get_cols() )
                I_patch.at<uchar>(i+stepx,j+stepy) = I.at<uchar>(p.x+i,p.y+j);
        }
    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;
    int ddepthC1 = CV_32SC1;

    double min, max;
    cv::minMaxLoc(I_patch, &min, &max);

    cv::Mat grad_x,grad_y;
    cv::Scharr(I_patch/max,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
    cv::Scharr(I_patch/max,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);

    cv::Mat magnitude(patch_size_x, patch_size_y, ddepthC1);
    for(int j=0; j<patch_size_y; ++j)
        for(int i=0; i<patch_size_x; ++i)
        {
            magnitude.at<float>(i,j)= sqrt(grad_x.at<float>(i,j)*grad_x.at<float>(i,j)+grad_y.at<float>(i,j)*grad_y.at<float>(i,j));
        }

    cv::Point2i max_loc,new_max_loc;

    cv::minMaxLoc(magnitude, &min, &max,NULL,&max_loc);
    magnitude.at<float>(max_loc) =0.0f;

    // Handle the case with 2 maximum positions
    //    double new_max;
    //    cv::minMaxLoc(magnitude, &min, &new_max,NULL,&new_max_loc);

    //    if(new_max==max)
    //    {
    //        float milieu =round(patch_size/2);
    //        float distance = sqrt((max_loc.x-milieu)*(max_loc.x-milieu)+(max_loc.y-milieu)*(max_loc.y-milieu));
    //        float distance_new = sqrt((new_max_loc.x-milieu)*(new_max_loc.x-milieu)+(new_max_loc.y-milieu)*(new_max_loc.y-milieu));
    //        if(distance >= distance_new)
    //            return new_max_loc;
    //    }

    return max_loc;
}

//void RegionFill::compute_isophotes()
//{
//    cv::Mat I;
//    cvtColor( im->image(), I, CV_RGB2GRAY );

//    int scale = 1;
//    int delta = 0;
//    int ddepth = CV_32F;
//    int ddepthC1 = CV_32SC1;

//    double min, max;
//    cv::minMaxLoc(I, &min, &max);

//    cv::Mat grad_x,grad_y;
//    //    cv::Sobel( I/max, grad_x, CV_32F, 1, 0, 3,scale, delta, cv::BORDER_DEFAULT);
//    //    cv::Sobel( I/max, grad_y, CV_32F, 0, 1, 3,scale, delta, cv::BORDER_DEFAULT);
//    cv::Scharr(I/max,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
//    cv::Scharr(I/max,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);
//    cv::Mat orientation(im->get_rows(), im->get_cols(), ddepthC1);
//    cv::Mat magnitude(im->get_rows(), im->get_cols(), ddepthC1);
//    for(int j= 0; j< grad_y.cols; ++j)
//        for(int i = 0; i < grad_y.rows; ++i)
//        {
//            orientation.at<float>(i,j) = atan2(grad_y.at<float>(i,j),grad_x.at<float>(i,j) ) * 180/M_PI ;
//            magnitude.at<float>(i,j)= sqrt(grad_x.at<float>(i,j)*grad_x.at<float>(i,j)+grad_y.at<float>(i,j)*grad_y.at<float>(i,j));
//        }

//    isophotes_data_magnitude = magnitude;
//    isophotes_data_orientation = orientation;
//}

void RegionFill::init_confidence()
{
    double min, max;
    cv::minMaxLoc(im->mask(), &min, &max);
    confidence = (im->mask()==0)/255;
}

float RegionFill::compute_confidence(cv::Point2i p)
{
    cv::Mat patch = cv::Mat::zeros(patch_size_x,patch_size_y,im->image().type());

    int stepx = floor(patch_size_x/2);
    int stepy = floor(patch_size_y/2);
    float conf=0.0f;

    for(int j=-stepy; j<=stepy; ++j)
        for(int i=-stepx; i<=stepx; ++i)
        {
            if(j>0 && j<im->get_cols())

                if(i>0 && i< im->get_rows())
                {
                    patch.at<cv::Vec3b>(i+stepx,j+stepy) = im->get_image_pixel(i+p.x,j+p.y); //why just im->image(i,j) doesn't work ?? we changed the prototype in image.hc
                    if(im->get_alpha_pixel(i+p.x,j+p.y) ==  SOURCE)
                        conf += confidence.at<uchar>(i+p.x,j+p.y);
                }
        }

    conf = conf/(patch_size_x*patch_size_y);
    return conf;
}


float RegionFill::compute_data_term(cv::Point p)
{

    float data_term;
    //Need to change y and x because of at in im->alpha(i,j)
    if(im->get_alpha_pixel(p.x,p.y) == BORDER)
    {
        cv::Mat gauss_x = cv::getGaussianKernel(3, 1 , CV_32F);
        cv::Mat gauss_y = cv::getGaussianKernel(3, 1, CV_32F);

        cv::Mat p_neighbors = cv::Mat::zeros(3,3,gauss_x.type());
        int step = floor(3/2);

        cv::Mat gauss = gauss_x*gauss_y.t();
        int size_x =  im->get_rows();
        int size_y =  im->get_cols();
        //std::cout << "size : " << size_x  <<" " << size_y;

        for(int j=-step; j<=step; ++j)
            for(int i=-step; i<=step; ++i)
            {
                if(((i+p.x)>0 && (i+p.x)<size_x)  && ((j+p.y)>0 && (j+p.y)<size_y) )
                {
                    if(im->get_alpha_pixel(i+p.x,j+p.y) == BORDER)
                    {
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
            if(im->get_alpha_pixel(x,y) == SOURCE || im->get_alpha_pixel(x,y)== UPDATED)
            {
                //                std::cout<<"Source ou UPDATED"<<std::endl;
                n_p = -n_p;
            }
            //            else if(im->alpha(x,y) == BORDER)
            //                std::cout<<"Erreur dans la direction de la normale"<<std::endl;
            //            else ;
        }


        //std::cout << std::setprecision(3)<< "n_p = [" << n_p.x <<","<< n_p.y<<"];" <<std::endl;

        cv::Point Ip =compute_isophotes(p);

        float alpha =255.0f;
        data_term = std::abs(Ip.x * n_p.y - Ip.y * n_p.x)/alpha;
        //        std::cout<<"Data term : " << data_term<<std::endl;

    }

    //float T = (successive.y - previous.y) / (successive.x - previous.x);

    return data_term; //debug
}

void RegionFill::compute_priority()
{
    if(get_border_size()==1)
    {
        border_point bp;
        bp.priority =100.0f;

        border[0]=bp;
    }
    else
    {
        for(border_point& bp : border)
        {
            //        std::cout << "border "<< bp.coord.x << " "<< bp.coord.y << std::endl;
            bp.priority = compute_confidence(bp.coord)*compute_data_term(bp.coord);
        }
    }
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
patch P;
cv::Point2i RegionFill::find_exemplar_patch(cv::Point2i p)
{
    cv::Point2i coord_center_patchQ;

    //    //Create and store patch P (only pixels in the source)

    //    //Create a patch Q
    //

    P = patch(im,patch_size_x,patch_size_y,p,true);
    patch Q(im,P.get_size().x,P.get_size().y);
////    cv::imshow("P",P.get_patch());
//    cv::waitKey(0);
//    //    std::cout << p.x << " " << p.y << std::endl;
    int stepx = floor(P.get_size().x/2);
    int stepy = floor(P.get_size().y/2);



    float distance_max = 10e+7;

    //Run through all patch of the image
    //Each patch tested must be completely in the source
    //Do we test only pixels of the patch corresponding to the source's pixels that were in the source ?

//    for(int v=0; v< im->get_cols()-1; v++)
//        for(int u=0; u< im->get_rows()-1; u++)
    for(int v=stepy+1; v< im->get_cols()-stepy-1; v++)
        for(int u=stepx+1; u< im->get_rows()-stepx-1; u++)
        {
            float distance = 0.0f;
            Q.set_center_and_fill(cv::Point2i(u,v),false);
////            std::cout << "u "<< u<<" v " <<v<< " lim "<< im->get_rows()-stepx-1<< std::endl;

////            patch Q(im,P.get_size().x,P.get_size().y,cv::Point2i(u,v),false);


                        if(Q.is_whole_patch_source()) //check if Q is all in the source
                        {

//                            // set to (-1,-1,-1) pixels of Q corresponding to non source pixel of P
//                            //so the comparison / distance is meaningful
                            Q.mask(P);
//            //                cv::imshow("Qpatch",Q.get_patch());
//            //                cv::waitKey(0);
//                            //compute distance
                            distance = P.compute_distance_SSD_LAB(Q);
//                            //                std::cout << distance << std::endl;
                            if(distance < distance_max && distance != 0.0f) //minimise la distance
                            {
                                distance_max = distance;
                                coord_center_patchQ = cv::Point2i(u,v);
                                //                    std::cout << distance <<" ";
                            }

                        }
        }

    return coord_center_patchQ;
}

/** Reimpmeneted in Patch but need to check its really working */

//float RegionFill::compute_patch_SSD_LAB(cv::Mat A,cv::Mat B)
//{
//    float dist = 0.0f;
//    int step = floor(patch_size/2);
//    for(int i=-step; i<=step; ++i)
//        for(int j=-step; j<=step; ++j)
//        {
//            //Get pixel (i,j) of both patch
//            cv::Mat3b pixP (A.at<cv::Vec3b>(i+step,j+step));
//            cv::Mat3b pixQ (B.at<cv::Vec3b>(i+step,j+step));
//            //            cv::Mat3b pixQ (im->get_image(u+i+step,v+j+step));

//            //Convert them to CIE Lab (L,a,b)
//            cv::Mat3b pixPLab, pixQLab;
//            cvtColor(pixP,pixPLab,cv::COLOR_RGB2Lab);
//            cvtColor(pixQ,pixQLab,cv::COLOR_RGB2Lab);
//            //Compute distance //Formula for CIE 76 // Update to CIE 94 (look wikipedia) if not working
//            float dL = pixPLab[0][0][0] - pixQLab[0][0][0];
//            float da = pixPLab[0][0][1] - pixQLab[0][0][1];
//            float db = pixPLab[0][0][2] - pixQLab[0][0][2];
//            dist += cv::sqrt(dL*dL + da*da + db*db);
//        }
//    return dist;
//}

void RegionFill::propagate_texture(cv::Point2i p, cv::Point2i q,int sizex,int sizey)
{
    cv::Mat Q = cv::Mat(im->image(),cv::Rect(q.x,q.y,sizex,sizey));
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);
    int x0=0,y0=0,x1=0,y1=0;
    if(p.x-stepx < 0)                { x0=0; }                 else { x0= p.x-stepx; }
    if(p.x+stepx >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= p.x+stepx; }
    if(p.y-stepy < 0)                { y0=0; }                 else { y0= p.y-stepy; }
    if(p.y+stepy >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= p.y+stepy; }
    std::cout << "x0 x1 y0 y1 :" << x0 << " "<< x1 << " " << y0 << " " << y1 << std::endl;
    std::cout << im->get_rows() << " " << im->get_cols() << std::endl;
    std::cout << "" << std::endl ;
    Q(cv::Range(0,sizex-1),cv::Range(0,sizey-1)).copyTo(im->image()(cv::Range(x0,x1),cv::Range(y0,y1)));
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


/** Perform the algorithm needed to copy texture
     * */
void RegionFill::run()
{

    //verif que l'image n'est pas nulle à faire avant de pouvoir lancer l'algo ...

    //    //init confidence
    init_confidence();
    //    //Compute priorities of the border points
    init_border();

    //    // While there are IN in alpha (all the mask hasn't been updated)
            while(!whole_image_processed())
//    for(int k=0; k< 3; k++)
    {

        /** 1.a */
        //Done with init_border then next by update alpha which update border stored in "border"
        /** 1.b */

        compute_priority();
        /** 2.a */
                cv::Point2i point_priority = running_through_patches();
        /** 2.b */
        //        find_exemplar_patch (minimizing d(Pp,Pq)

//        cv::Point2i point_priority = cv::Point2i(0,0);
        std::cout << "point priority " << point_priority << std::endl;
        cv::Point2i point_exemplar = find_exemplar_patch(point_priority);
        std::cout << "#########"<<std::endl;
        //        cv::Point2i point_exemplar = cv::Point2i(113,156);
                std::cout <<"point exemplar " << point_exemplar << std::endl;
        /** 2.c */
        //propagate_texture
                propagate_texture(point_priority, point_exemplar,P.get_size().x,P.get_size().y);
//        /** 3 */
        std::cout << get_border_size() << std::endl;
//        /** Update alpha, border, and confidence in the patch arround the border_point computed */
                update_alpha(point_priority, P.get_size().x,P.get_size().y); //Actually : update alpha, border, confidence

    }
    std::cout << get_border_size() << std::endl;
    im->imwrite("result.png");
    exit(0);




}

