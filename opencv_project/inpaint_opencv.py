import numpy as np 
import cv2

#LENA
#img = cv2.imread('../images/originales/lena_color_256.png')
#d_img = cv2.imread('../images/standard_test_images/mask_lena_color_256.png')


#img = cv2.imread('../images/standard_test_images/d_peppers_color.png')
#mask = cv2.imread('../images/standard_test_images/mask_peppers_color.png')

#LINCOLN
img = cv2.imread("../images/originales/lincoln.jpg")
#d_img = cv2.imread("../images/d_lincoln.jpg")



mask = cv2.imread("../images/mask/linc-mask.bmp")

ret,thresh1 = cv2.threshold(mask,1,255,cv2.THRESH_BINARY)
mask3 = cv2.cvtColor(thresh1,cv2.COLOR_RGB2GRAY, 1)
dst = cv2.inpaint(img,mask3 ,3,cv2.INPAINT_TELEA) #INPAINT_TELEA or INPAINT_NS


cv2.imshow('Originale', img)
cv2.imshow('Mask',mask3)
cv2.imshow('dst',dst)
cv2.waitKey(0)
cv2.destroyAllWindows()

cv2.imwrite('resultLinc2OpenCV.jpg',dst)

