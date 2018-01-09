clear all; clc;
img = imread('oval.png');
img = rgb2gray(img);
[I, theta] = isophote(img,.25);
subplot(1,2,1), imshow(I), title('Magnitude of the isophotes');
subplot(1,2,2), imagesc(theta), title('Theta');