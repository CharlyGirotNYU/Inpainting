import numpy as np 
import cv2

img = cv2.imread('../images/standard_test_images/d_lena_color_256.tif')
mask = cv2.imread('../images/standard_test_images/mask_d_lena_color_256.png',0)
#img = cv2.imread('../images/standard_test_images/d_peppers_color.png')
#mask = cv2.imread('../images/standard_test_images/mask_peppers_color.png')

dst = cv2.inpaint(img,mask,3,cv2.INPAINT_NS)

cv2.imshow('dst',dst)
cv2.waitKey(0)
cv2.destroyAllWindows()

