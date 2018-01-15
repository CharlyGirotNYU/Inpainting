function [ droite ] = correct_d( d )
%UNTITLED3 Summary of this function goes here
%   Detailed explanation goes here

if(d(1,2)==0)
    d(1,2)=2;
end
if(d(1,2)==2)
    d(1,2)=0;
end
    if(d(2 ,2)==0)
    d(1,2)=2;
    end
if(d(2,2)==2)
    d(1,2)=0;
end
    

droite = [d(1,1)+1,d(1,2)+1;d(2,1)+1,d(2,2)+1];

end

