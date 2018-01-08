#include "image.hpp"


int main()
{
//TEST CHARLY
    Image image = Image();
    image.imread("../../images/lincoln.jpg","../../images/mask_lincoln.jpg");

    image.imwrite("test.jpg");
    return 0;
}
