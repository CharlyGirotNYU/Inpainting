% EXTRACTING ISOPHOTES FROM AN IMAGE
%
% Usage:  [ I, theta ] = isophote( L, alpha );
%
% Argument:   L     -  Luminance of the input image [0 255]
%             alpha -  Threshold of isophotes       [0 1] 
%               
% Returns:    I     -  Magnitude of the isophotes   [0 1]
%             theta -  Angle of the isophotes   [-pi/2 +pi/2] 

% Vahid. K. Alilou
% Department of Computer Engineering
% The University of Semnan
%
% December 2014

function [ I, theta ] = isophote( L, alpha )
    L = double(L)/255;
    theta=zeros(size(L));
    [Lx,Ly] = gradient(L);
    I = sqrt(Lx.^2+Ly.^2);
    I = I./max(max(I));
    T = I>=alpha;
    theta(T) = atan(Ly(T)./Lx(T));
    I(I<alpha)=0;
end