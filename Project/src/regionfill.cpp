#include "regionfill.hpp"
#include <iomanip>
#include "patch.hpp"

patch P;

RegionFill::RegionFill()
{
    patch_size_x=7; //has to be impaired
    patch_size_y=7;
}



void RegionFill::init_confidence()
{
    double min, max;
    cv::minMaxLoc(im->mask(), &min, &max);
    confidence = cv::Mat::zeros(im->get_rows(),im->get_cols(),CV_32FC1);

    cv::Mat a = (im->mask() == SOURCE);
    for(int j=0; j <im->get_cols(); j++)
        for(int i=0; i<im->get_rows(); i++)
        {
            if(a.at<uchar>(i,j) !=0)
                confidence.at<float>(i,j)=1.0f;
        }
}

/** get and fill the border */
void RegionFill::init_border()
{
    for(int j=0; j<im->alpha().cols; ++j)
    {
        for(int i=0; i<im->alpha().rows; ++i)
        {
            if(im->get_alpha_pixel(i,j) == BORDER)
            {
                border_point point;
                point.coord = cv::Point2i(i,j);
                point.priority = 0.0f;
                border.push_back(point);
            }
        }
    }

    //    cv::waitKey(0);
}

void RegionFill::compute_priority()
{
    std::cout<<"Border size "<<get_border_size()<<std::endl;
    if(get_border_size()==1)
    {
        border_point bp = border[0];
        bp.priority =1000.0f;
        border[0]=bp;
    }
    else
    {
        cv::Mat bordure = cv::Mat::zeros(im->image().size[0],im->image().size[1],CV_32FC1);
        cv::Mat data_term = cv::Mat::zeros(im->image().size[0],im->image().size[1],CV_32FC1);
        //cv::Mat confidence = cv::Mat::zeros(im->image().size[0],im->image().size[1],CV_32F);

        for(border_point& bp : border)
        {
            // bp.priority = compute_confidence(bp.coord)*compute_data_term(bp.coord);

            data_term.at<float>(bp.coord.x,bp.coord.y) = compute_data_term(bp.coord);
            confidence.at<float>(bp.coord.x,bp.coord.y) = compute_confidence(bp.coord);
            bordure.at<float>(bp.coord.x,bp.coord.y) = 255.0f;
            bp.priority =  data_term.at<float>(bp.coord.x,bp.coord.y) * confidence.at<float>(bp.coord.x,bp.coord.y);


            //            std::cout << compute_confidence(bp.coord)<<" "<< compute_data_term(bp.coord)<<std::endl;
            //            std::cout<<bp.coord<<std::endl;
            //            if(bp.coord.x == 0)std::cout << bp.priority << std::endl;

            //            std::cout << 200000000*compute_data_term(bp.coord) << std::endl;
            //            im->set_image_pixel(bp.coord.x,bp.coord.y) = cv::Vec3b(0,255*compute_confidence(bp.coord),0);
        }
        cv::namedWindow("data term",cv::WINDOW_NORMAL);
        cv::imshow("data term",data_term*10);
        //            cv::waitKey(0);
        cv::namedWindow("confidence",cv::WINDOW_NORMAL);
        cv::imshow("confidence",confidence);
        cv::namedWindow("bordure",cv::WINDOW_NORMAL);
        cv::imshow("bordure",bordure);
        cv::waitKey(10);

    }

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
            if(j+p.y>0 && j+p.y<im->get_cols()-1)

                if(i+p.x>0 && i+p.x< im->get_rows()-1)
                {
                    //                    //patch.at<cv::Vec3b>(i+stepx,j+stepy) = im->get_image_pixel(i+p.x,j+p.y); //why just im->image(i,j) doesn't work ?? we changed the prototype in image.hc
                    if(im->get_alpha_pixel(i+p.x,j+p.y) ==  SOURCE)
                    {
                        conf += confidence.at<float>(i+p.x,j+p.y);
                    }
                }
        }
    conf = conf/(patch_size_x*patch_size_y);
    return conf;
}

float RegionFill::compute_data_term(cv::Point2i p)
{

    float data_term=0.0f;
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
//            std::cout << x << y << std::endl;
            if(im->get_alpha_pixel(x,y) == SOURCE || im->get_alpha_pixel(x,y)== UPDATED)
            {
//                std::cout<<"SOURCE OU UPDATED";
                n_p = -n_p;
            }
        }

        cv::Point2f Ip =compute_isophotes(p);
        float alpha =255.0f;
        data_term = std::abs(Ip.x * n_p.y - Ip.y * n_p.x)/alpha;
//        std::cout << Ip << n_p <<std::endl;
    }
    return data_term; //debug
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

    return cv::Point2f(n_p.y/norm, n_p.x/norm);
}

/** Running through patches along the border
    * Maybe we shall return a border point and not a cv::Point2i */
cv::Point2i RegionFill::running_through_patches()
{
    float priority = 0.0f;
    float store=0.0f; // Only to show the priority of the point
    cv::Point2i coord_priority;
    for( border_point& bPoint : border)
    {

        if(bPoint.priority >= priority)
        {
            coord_priority = bPoint.coord;
            priority = bPoint.priority;

            store=bPoint.priority;
        }
    }
    std::cout<<"Piority "<<store<<std::endl;
    return coord_priority;
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
    cv::Point2i coord_center_patchQ;

    P = patch(im,patch_size_x,patch_size_y,p,true);
    patch Q = patch(im,P.get_size().x,P.get_size().y);

    //    std::cout << "taille patch P apres : " << P.get_size().x << " " << P.get_size().y << std::endl;
    //    std::cout << "taille patch Q apres : " << Q.get_size().x << " " << Q.get_size().y << std::endl;
    int stepx = floor(P.get_size().x/2);
    int stepy = floor(P.get_size().y/2);

    //    std::cout << "step " << stepx << " " << stepy << std::endl;

    float distance_max = 10e+7;

    //    for(int v=0; v< im->get_cols()-1; v++)
    //        for(int u=0; u< im->get_rows()-1; u++)
    for(int v=stepy+1; v< im->get_cols()-stepy-1; v++)
        for(int u=stepx+1; u< im->get_rows()-stepx-1; u++)
        {
            float distance = 0.0f;
            bool valid = Q.set_center_and_fill(cv::Point2i(u,v),false);

            //            std::cout << "taille patch Q apres remplissage : " << Q.get_size().x << " " << Q.get_size().y << std::endl;
            if(valid == true)
            {
                if(Q.is_whole_patch_source()) //check if Q is all in the source
                {
                    //                std::cout << "coordonnées patch Q : " << u << " " << v << std::endl;
                    Q.mask(P,true); //true ==> get points in the source
                    distance = P.compute_distance_SSD_LAB(Q);

                    if(distance <= distance_max)// && distance != 0.0f) //minimise la distance
                    {
                        distance_max = distance;
                        coord_center_patchQ = cv::Point2i(u,v);
                    }

                }
            }
        }
    std::cout << "distance max " << distance_max << std::endl;

    return coord_center_patchQ;
}


void RegionFill::propagate_texture(cv::Point2i p, cv::Point2i q,int sizex,int sizey)
{
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    int x0=0,y0=0,x1=0,y1=0;
    if(p.x-stepx < 0)                { x0=0; }                 else { x0= p.x-stepx; }
    if(p.x+stepx >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= p.x+stepx; }
    if(p.y-stepy < 0)                { y0=0; }                 else { y0= p.y-stepy; }
    if(p.y+stepy >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= p.y+stepy; }

    cv::Mat Q = im->image()(cv::Range(q.x-stepx,q.x+stepx),cv::Range(q.y-stepy,q.y+stepy));//sizex,sizey));

    sizex = x1-x0+1;
    sizey = y1-y0+1;

    std::cout << "p " << p << std::endl;
    std::cout << "q " << q << std::endl;
    std::cout << "x0 x1 " << x0 << " " << x1 <<std::endl;
    std::cout << "y0 y1 " << y0 << " " << y1 <<std::endl;
    cv::Mat mask_P_source = (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == SOURCE)
            + (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == UPDATED);
    cv::Mat mask_P_in = (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == BORDER)
            + (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == IN);

    //    cv::imshow("Q",mask_P_source);
    //    cv::waitKey(0);
    //    cv::imshow("Q",mask_P_in);
    //    cv::waitKey(0);

    cv::Mat P_source = cv::Mat::zeros(sizex,sizey,im->image().type());
    cv::Mat Q_in = cv::Mat::zeros(sizex,sizey,im->image().type());



    im->image()(cv::Range(x0,x1),cv::Range(y0,y1)).copyTo(P_source, mask_P_source);
    Q.copyTo(Q_in,mask_P_in);

    cv::Mat new_patch = cv::Mat::zeros(sizex,sizey,im->image().type());
    cv::add(P_source,Q_in,new_patch);
    //    new_patch = P_source + Q_in ;
    //    cv::add(P_source,Q,new_patch);
    //    im->image()(cv::Range(x0,x1),cv::Range(y0,y1)).copyTo(Q_in, mask_P_in);

    //    cv::imshow("Q",P_source);
    //    cv::waitKey(0);
    //    cv::imshow("Q",Q_in);
    //    cv::waitKey(0);
    //    cv::imshow("Q",new_patch);
    //    cv::waitKey(0);
    //    std::cout << Q_in.at<cv::Vec3b>(14,0) <<std::endl;



    //    patch P(im->image(),)
    //    cv::Mat alpha_Q = cv::Mat::zeros(sizex,sizey,im->alpha().type());
    //    cv::Mat mask_a = cv::Mat::zeros(sizex,sizey,im->alpha().type());
    //    cv::Mat mask_alpha = cv::Mat(im->alpha(),cv::Rect(x0,y0,sizex,sizey));

    //    mask_a = (mask_alpha == IN) + (mask_alpha == BORDER);
    //    Q.copyTo(alpha_Q,mask_a);
    //    alpha_Q(cv::Range(0,sizex-1),cv::Range(0,sizey-1)).copyTo(im->image()(cv::Range(x0,x1),cv::Range(y0,y1)));
    //    new_patch.copyTo(im->image()(cv::Range(x0,x1),cv::Range(y0,y1)));
    new_patch.copyTo(im->image()(cv::Range(x0,x1),cv::Range(y0,y1)));
}


cv::Point2f RegionFill::compute_isophotes(cv::Point2i p)
{
    cv::Mat I; cvtColor(im->image(), I, CV_RGB2GRAY);

    int sizex = patch_size_x;// P.get_size().x;
    int sizey = patch_size_y;//P.get_size().y;
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    int x0=0,y0=0,x1=0,y1=0;
    if(p.x-stepx < 0)                { x0=0; }                 else { x0= p.x-stepx; }
    if(p.x+stepx >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= p.x+stepx; }
    if(p.y-stepy < 0)                { y0=0; }                 else { y0= p.y-stepy; }
    if(p.y+stepy >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= p.y+stepy; }

    cv::Mat carre = cv::Mat::zeros(sizex,sizey,I.type());

    carre = I(cv::Range(x0,x1),cv::Range(y0,y1));

    int scale = 1;
    int delta = 0;
    int ddepth = CV_32F;

    double min, max;
    max=0;min=0;
    cv::minMaxLoc(carre, &min, &max);

    cv::Mat grad_x = cv::Mat::zeros(sizex,sizey,ddepth);
    cv::Mat grad_y = cv::Mat::zeros(sizex,sizey,ddepth);
    cv::Scharr(carre/max,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
    cv::Scharr(carre/max,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);

    cv::Mat grad_x2 = cv::Mat::zeros(sizex,sizey,ddepth);
    cv::Mat grad_y2 = cv::Mat::zeros(sizex,sizey,ddepth);

    cv::pow(grad_x,2,grad_x2);
    cv::pow(grad_y,2,grad_y2);

    cv::Mat grad = cv::Mat::zeros(sizex,sizey,CV_32F);
    cv::sqrt(grad_x2+grad_y2,grad);
    //    cv::imshow("patch",grad);
    //    cv::waitKey(0);


    cv::Point2i max_loc,new_max_loc;
    cv::minMaxLoc(grad, &min, &max,NULL,&max_loc);
    //Marche pour l'instant mais bancal
    cv::Point2f Ip(-grad_y2.at<float>(max_loc),grad_x2.at<float>(max_loc));
//    cv::Point2f Ip(-grad_y.at<float>(max_loc),grad_x.at<float>(max_loc));
    return Ip;

}

//cv::Point2f RegionFill::compute_isophotes(cv::Point2i p)
//{
//    cv::Mat I;
//    cvtColor( im->image(), I, CV_RGB2GRAY );

//    int stepx = floor(patch_size_x/2);
//    int stepy = floor(patch_size_y/2);

//    cv::Mat I_patch(patch_size_x,patch_size_y,0);
//    for(int j=-stepy; j<=stepy; ++j)
//        for(int i=-stepx; i<=stepx; ++i)
//        {
//            if(p.x+i > 0 && p.x +i < im->get_rows() &&  p.y+j > 0 && p.y +j < im->get_cols() )
//                if((im->get_alpha_pixel(i,j)==SOURCE) || (im->get_alpha_pixel(i,j)==UPDATED)  )
//                I_patch.at<uchar>(i+stepx,j+stepy) = I.at<uchar>(p.x+i,p.y+j);
//            else
//                    I_patch.at<uchar>(i+stepx,j+stepy) = 0.0f;
//        }
//    int scale = 1;
//    int delta = 0;
//    int ddepth = CV_32F;
//    int ddepthC1 = CV_32SC1;

//    double min, max;
//    cv::minMaxLoc(I_patch, &min, &max);

//    cv::Mat grad_x,grad_y;
//    cv::Scharr(I_patch/max,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
//    cv::Scharr(I_patch/max,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);

//    cv::Mat magnitude(patch_size_x, patch_size_y, ddepthC1);
//    for(int j=0; j<patch_size_y; ++j){
//        for(int i=0; i<patch_size_x; ++i)
//        {
//                magnitude.at<float>(i,j)= sqrt(grad_x.at<float>(i,j)*grad_x.at<float>(i,j)+grad_y.at<float>(i,j)*grad_y.at<float>(i,j));
//        }
//    }
//cv::imshow("patch",magnitude);
//cv::waitKey(0);
//    max=0;min=0;
//    cv::Point2i max_loc,new_max_loc;

//    cv::minMaxLoc(magnitude, &min, &max,NULL,&max_loc);
//    cv::Point2f Ip(grad_x.at<float>(max_loc),grad_y.at<float>(max_loc));

//    return Ip;
//}


/** Check if the whole image has been processed
     * Takes a lot of time , mabe use iterator to gain efficiency
     * */
bool RegionFill::whole_image_processed()
{
    if(get_border_size() == 0)
        return true;
    return false;
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
    //    std::cout << "x0 x1 y0 y1 :" << x0 << " "<< x1 << " " << y0 << " " << y1 << std::endl;

    float cf = compute_confidence(bp);
    for(int j=y0; j<=y1; ++j)
        for(int i=x0; i<=x1; ++i)
        {
            if(im->get_alpha_pixel(i,j) != SOURCE)
            {
                //                im->set_alpha_pixel(i,j) = UPDATED;
                update_border( cv::Point2i(i,j),UPDATED);
                //update confidence
                if((i!=bp.x) && (j!= bp.y));
                    confidence.at<float>(i,j) = cf;// confidence.at<float>(bp.x,bp.y);
            }
        }

    //Taking care of the angles
    if(bp.x-stepx-1 < 0)                { x0=0; }                 else { x0= bp.x-stepx-1; }
    if(bp.x+stepx+1 >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= bp.x+stepx+1; }
    if(bp.y-stepy-1 < 0)                { y0=0; }                 else { y0= bp.y-stepy-1; }
    if(bp.y+stepy+1 >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= bp.y+stepy+1; }

    //Y A UN PROBLEME AVEC CES DEUX BOUCLES , LA BORDURE NE SE REMPLIT PAS BIEN APRES UN CHANGEMENT


    for(int i = x0; i<=x1; ++i)
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
    for(int j=y0; j<=y1; ++j)
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
    b.priority =0.0f;
    if(status == BORDER)
    {
        border.push_back(b);
        //Update the alpha value
        im->set_alpha_pixel(b.coord.x,b.coord.y) = BORDER;
//        confidence.at<float>(b.coord.x,b.coord.y) = compute_confidence(b.coord);
    }
    else if(status == UPDATED)
    {
        //Update the alpha value
        im->set_alpha_pixel(b.coord.x,b.coord.y) = UPDATED;
        //        std::cout << "border point to erase : " << b.coord << std::endl;
        int i=0;
        for(auto bp : border)
        {
            if(bp.coord == b.coord)
            {
                //                std::cout << " border coord to erase : " << b.coord << std::endl;
                border.erase(border.begin()+i);
                //im->set_image_pixel(bp.coord.x,bp.coord.y) -= cv::Vec3b(0,100,0);
            }
            i++;
        }
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

    return true; //TODO : Add for default but we need to Check
}





void RegionFill::test_compute_data_term()
{
    // Cas 0 : 187,101 // Cas 0 inverse : 190,122
    cv::Point cas_0(101,187);
    cv::Point cas_inv_0(122,190);

    // Cas 1 : 91, 117 // Cas 1 :inverse 69 112
    cv::Point cas_1(117,91);
    cv::Point cas_inv_1(112,69);

    // Cas 2 : 93 , 80  // Cas 2 inverse 70 115
    cv::Point cas_2(80,93);
    cv::Point cas_inv_2(115,70);
    // Cas 3 :  347 90 //  Cas 3 inverse 367 159
    cv::Point cas_3(90,347);
    cv::Point cas_inv_3(159,367);

    // Cas 4 : 351 ,  100 // Cas 4 inverse 171 101
    cv::Point cas_4(351,100);
    cv::Point cas_inv_4(171,101);

    // Cas 5 : 365 , 99   Cas 5 inverse 70,102
    cv::Point cas_5(365,99);
    cv::Point cas_inv_5(70,102);



    //    std::cout<<"Debut cas NORMAL"<<std::endl;
    compute_data_term(cas_3);
    //    std::cout<<std::endl;
    //    std::cout<<"Debut cas INVERS"<<std::endl;
    compute_data_term(cas_inv_3);

}


/** Perform the algorithm needed to copy texture* */
void RegionFill::run()
{

    init_confidence();
    init_border();
    while(!whole_image_processed())
//        //            for(int k=0; k< 3; k++)
    {


        compute_priority();
        cv::Point2i point_priority = running_through_patches();
        //        for(auto b : border)
        //            std::cout << b.data_term << " ";
        std::cout <<  "point priority " << point_priority << std::endl;
        cv::Point2i point_exemplar = find_exemplar_patch(point_priority);
        std::cout << "point exemplar "  << point_exemplar << std::endl;
        propagate_texture(point_priority, point_exemplar, P.get_size().x, P.get_size().y);
        update_alpha(point_priority, P.get_size().x, P.get_size().y); //Actually : update alpha, border, confidence

        //        border.clear();
        //        init_border();
        //        for(auto b : border)
        {
            //        im->set_image_pixel(b.coord.x,b.coord.y) = cv::Vec3b(0,0,125);
            //        im->set_alpha_pixel(b.coord.x,b.coord.y) = 4;
        }
        im->imwrite("result2.bmp");

    }
        im->imwrite("result.png");
    //    exit(0);


//    compute_priority();
//    cv::Point2i point_priority = cv::Point2i(196,132);
//    cv::Point2i exemplar = find_exemplar_patch(point_priority);
//    std::cout << exemplar << std::endl;
//    propagate_texture(point_priority, exemplar, P.get_size().x, P.get_size().y);
//    im->imwrite("result2.bmp");
    //    196,132
}

