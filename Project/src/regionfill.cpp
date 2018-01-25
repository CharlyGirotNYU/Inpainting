/** Regionfill.cpp
 * Inpainting project
 * Developed by Di Folco Maxime, Girot Charly, Jallais Maëliss
 * January 2018
 * CPE Lyon - 5ETI IMI
 * End of semester Project
 *
 * Based on the work and paper :
 * Criminisi, Antonio, Patrick Pérez, and Kentaro Toyama.
 * "Region filling and object removal by exemplar-based image inpainting."
 * IEEE Transactions on image processing 13.9 (2004): 1200-1212.
 * */

#include "regionfill.hpp"
#include <iomanip>
#include "patch.hpp"

patch P;

RegionFill::RegionFill()
{
    patch_size_x=65; //has to be impaired
    patch_size_y=65;
}

RegionFill::~RegionFill()
{}

RegionFill::RegionFill(int x,int y):
    patch_size_x(x),patch_size_y(y){}

/** Init confidence term of the whole image contained in im and store result in confidence*/
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

/** get and fill the border of the mask  */
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
}

/** Compute priority for all point belonging to the border of mask */
void RegionFill::compute_priority()
{
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

        for(border_point& bp : border)
        {
            data_term.at<float>(bp.coord.x,bp.coord.y) = compute_data_term(bp.coord);
            confidence.at<float>(bp.coord.x,bp.coord.y) = compute_confidence(bp.coord);
            bordure.at<float>(bp.coord.x,bp.coord.y) = 255.0f;
            bp.priority =  data_term.at<float>(bp.coord.x,bp.coord.y) * confidence.at<float>(bp.coord.x,bp.coord.y);

        }
        cv::namedWindow("data term",cv::WINDOW_NORMAL);
        cv::imshow("data term",data_term*10);
        cv::namedWindow("confidence",cv::WINDOW_NORMAL);
        cv::imshow("confidence",confidence);
        cv::namedWindow("bordure",cv::WINDOW_NORMAL);
        cv::imshow("bordure",bordure);
        cv::waitKey(20);
    }
}

/** Compute confidence term of a point using patch arround it*/
float RegionFill::compute_confidence(cv::Point2i p)
{
    int stepx = floor(patch_size_x/2);
    int stepy = floor(patch_size_y/2);

    float conf=0.0f;
    int nb_pixels =0;

    for(int j=-stepy; j<=stepy; ++j)
        for(int i=-stepx; i<=stepx; ++i)
        {
            if(j+p.y>0 && j+p.y<im->get_cols()-1)
                if(i+p.x>0 && i+p.x< im->get_rows()-1)
                    if(im->get_alpha_pixel(i+p.x,j+p.y) ==  SOURCE)
                    {
                        conf += confidence.at<float>(i+p.x,j+p.y);
                        nb_pixels++;
                    }
        }
    conf = conf/(patch_size_x*patch_size_y);
    return conf;
}

/** Compute data_term of a point using patch arround it */
float RegionFill::compute_data_term(cv::Point2i p)
{
    float data_term=0.0f;
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
                    if(im->get_alpha_pixel(i+p.x,j+p.y) == BORDER)
                        p_neighbors.at<float>(i+step,j+step) = gauss.at<float>(i+step,j+step);
            }

        cv::Point2f n_p =  compute_vector_normal(p,p_neighbors);
        // Verification de la direction de la normale \\Finally not usefull cause scalar product is absolute in the final compute
        int x,y;
        x = (int)(round(n_p.x)) + p.x;
        y = (int)(round(n_p.y)) + p.y;
        if(y>0 && y<size_y  && x>0 && x<size_x)
            if(im->get_alpha_pixel(x,y) == SOURCE || im->get_alpha_pixel(x,y)== UPDATED)
                n_p = -n_p;

        cv::Point2f Ip =compute_isophotes(p);
        float alpha =255.0f;
        data_term = std::abs(Ip.x * n_p.x + Ip.y * n_p.y)/alpha;
    }
    return data_term;
}


/** Compute the vector n_p from the point p and his nieghbors */
cv::Point2f RegionFill::compute_vector_normal(cv::Point p,  cv::Mat p_neighbors)
{
    p_neighbors.at<float>(1,1) = -1.0f;
    cv::Point max_loc1,max_loc2;
    double max;

    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc1);
    cv::Point pre = max_loc1;

    p_neighbors.at<float>(max_loc1) = -1.0f;
    cv::minMaxLoc(p_neighbors,NULL,&max,NULL,&max_loc2);

    cv::Point suc = max_loc2;
    p_neighbors.at<float>(max_loc2) = -1.0f;

    //Compute the middle of the line between previous and successive
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
            point.y +=1 ;//horizontal case
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

/** Running through patches along the border to find the border point of highest priority */
cv::Point2i RegionFill::running_through_patches()
{
    float priority = .0f;
    cv::Point2i coord_priority;
    for( border_point& bPoint : border)
    {
        if(bPoint.priority >= priority)
        {
            coord_priority = bPoint.coord;
            priority = bPoint.priority;
        }
    }
    return coord_priority;
}

/** find exemplar patch :
     * We search in the source region for the patch which is most similar to Pp.
     * d(Pp,Pq) is the sum of squaerd differences SSD of the ALREADY filled pixels in the two patches
     * We use the CIE Lab colour space because of its property of perceptual information
     * Receive : center point of the reference patch P
     * Return : the center point of the exemplar patch Q
     * */
cv::Point2i RegionFill::find_exemplar_patch(cv::Point2i p)
{
    cv::Point2i coord_center_patchQ;

    P = patch(im,patch_size_x,patch_size_y,p,true);
    patch Q = patch(im,P.get_size().x,P.get_size().y);

    int stepx = floor(P.get_size().x/2);
    int stepy = floor(P.get_size().y/2);
    float distance_max = 10e+7;

    //TODO : Not cheCk the whole image but rather a region (big enough to find a suitable correspondance) arround the patch
    for(int v=stepy+1; v< im->get_cols()-1-stepy; v=v+stepy)
        for(int u=stepx+1; u< im->get_rows()-1-stepx; u=u+stepx)
        {
            float distance = 0.0f;
            bool valid = Q.set_center_and_fill(cv::Point2i(u,v),false);
            if(valid == true)
            {
                if(Q.is_whole_patch_source()) //check if Q is all in the source
                {
                    distance = P.compute_distance_SSD_LAB(Q);
                    if(distance < distance_max)//minimise la distance
                    {
                        distance_max = distance;
                        coord_center_patchQ = cv::Point2i(u,v);
                    }
                }
            }
        }
    return coord_center_patchQ;
}

/** Propagate texture from exemplar Patch to patch on the border */
void RegionFill::propagate_texture(cv::Point2i p, cv::Point2i q, int sizex, int sizey)
{
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    //TODO : adapt it in case the patch is on the border of the image and so not whole
    if(p.x-stepx < 0 || p.x+stepx >= im->get_rows() || p.y-stepy < 0 || p.y+stepy >= im->get_cols())
    {
        std::cout<<"Error in propagate_texture : patch too big"<<std::endl;
        return;
    }

    for(int j=0;j<sizey;++j)
        for(int i=0; i<sizex;++i)
        {
            if(im->get_alpha_pixel(p.x-stepx+i,p.y-stepy+j)==IN ||im->get_alpha_pixel(p.x-stepx+i,p.y-stepy+j)==BORDER)//  ||im->get_alpha_pixel(p.x-stepx+i,p.y-stepy+j)==UPDATED)
                im->set_image_pixel(p.x-stepx+i,p.y-stepy+j) = im->get_image_pixel(q.x-stepx+i,q.y-stepy+j);
        }
}

/** Compute Isophote of the image
*   -L : Luminance of the input image
*   -alpha : Threshold of isophotes */
cv::Point2f RegionFill::compute_isophotes(cv::Point2i p)
{
    cv::Mat I; cvtColor(im->image(), I, CV_RGB2GRAY);

    int sizex = patch_size_x;
    int sizey = patch_size_y;
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
    cv::Scharr(carre/255.0f,grad_x,ddepth,1,0,scale,delta,cv::BORDER_DEFAULT);
    cv::Scharr(carre/255.0f,grad_y,ddepth,0,1,scale,delta,cv::BORDER_DEFAULT);

    cv::Mat grad_x2 = cv::Mat::zeros(sizex,sizey,ddepth);
    cv::Mat grad_y2 = cv::Mat::zeros(sizex,sizey,ddepth);

    cv::pow(grad_x,2,grad_x2);
    cv::pow(grad_y,2,grad_y2);

    cv::Mat grad = cv::Mat::zeros(sizex,sizey,CV_32F);

    cv::sqrt(grad_x2+grad_y2,grad);


    cv::Point2i max_loc;
    cv::Mat mask_source = cv::Mat::zeros(sizex,sizey,CV_8U);
    mask_source = (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == SOURCE) + (im->alpha()(cv::Range(x0,x1),cv::Range(y0,y1)) == UPDATED);

    for(int i=0; i<sizex-1; ++i)
        for(int j=0; j<sizey-1; ++j)
        {
            if(im->get_alpha_pixel(x0+i+1, y0+j) == BORDER || im->get_alpha_pixel(x0+i-1, y0+j) == BORDER
                    || im->get_alpha_pixel(x0+i, y0+j+1) == BORDER || im->get_alpha_pixel(x0+i, y0+j-1) == BORDER)
                mask_source.at<uchar>(i,j)=0; // Cette ligne fait planter le process pour certaines images : raisons inconnues , des fois ca passe des fois ca passe pas.
        }

    cv::Mat grad_source = cv::Mat::zeros(sizex,sizey,CV_32F);
    grad.copyTo(grad_source,mask_source);

    cv::minMaxLoc(grad_source, NULL, &max, NULL, &max_loc);
    //Marche pour l'instant mais bancal
    cv::Point2f Ip(-grad_x.at<float>(max_loc),grad_y.at<float>(max_loc));
    return Ip;
}


/** Check if the whole image has been processed
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
    int stepx = floor(sizex/2);
    int stepy = floor(sizey/2);

    int x0=0,y0=0,x1=0,y1=0;
    if(bp.x-stepx < 0)                { x0=0; }                 else { x0= bp.x-stepx; }
    if(bp.x+stepx >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= bp.x+stepx; }
    if(bp.y-stepy < 0)                { y0=0; }                 else { y0= bp.y-stepy; }
    if(bp.y+stepy >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= bp.y+stepy; }

    float cf = compute_confidence(bp);
    for(int j=y0; j<=y1; ++j)
        for(int i=x0; i<=x1; ++i)
        {
            if(im->get_alpha_pixel(i,j) != SOURCE && im->get_alpha_pixel(i,j) != UPDATED)
            {
                update_border( cv::Point2i(i,j),UPDATED);
                //update confidence
                confidence.at<float>(i,j) = cf;
            }
        }

    //Taking care of the angles
    if(bp.x-stepx-1 < 0)                { x0=0; }                 else { x0= bp.x-stepx-1; }
    if(bp.x+stepx+1 >= im->get_rows())  { x1= im->get_rows()-1; } else { x1= bp.x+stepx+1; }
    if(bp.y-stepy-1 < 0)                { y0=0; }                 else { y0= bp.y-stepy-1; }
    if(bp.y+stepy+1 >= im->get_cols())  { y1= im->get_cols()-1; } else { y1= bp.y+stepy+1; }

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
        im->set_alpha_pixel(b.coord.x,b.coord.y) = BORDER;
    }
    else if(status == UPDATED)
    {
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

/** Perform the algorithm needed to copy texture **/
void RegionFill::run()
{
    init_confidence();
    init_border();
    while(!whole_image_processed())
    {
        compute_priority();
        cv::Point2i point_priority = running_through_patches();
        cv::Point2i point_exemplar = find_exemplar_patch(point_priority);

        propagate_texture(point_priority, point_exemplar, P.get_size().x, P.get_size().y);
        update_alpha(point_priority, P.get_size().x, P.get_size().y); //Actually : update alpha, border, confidence

        cv::namedWindow("Resultat Actuel",cv::WINDOW_NORMAL);
        cv::imshow("Resultat Actuel",im->image()); cv::waitKey(1);
    }
}
