#include "image.hpp"
#include "regionfill.hpp"

int main()
{
//TEST CHARLY
    Image image = Image();
    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.png");

    image.imwrite("test.jpg");
    RegionFill region = RegionFill();
    region.set_image(&image);
    region.compute_isophotes(0.8);
    return 0;
}
