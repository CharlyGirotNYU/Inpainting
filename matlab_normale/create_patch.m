function [ patch ] = create_patch( cas )

% 0 0 0
% - - -
% 0 0 0
if(cas == 0)
    patch = [255,255,255;
             140,140,140;
             0  ,0  ,0  ];       
end


if(cas == 1)
    patch = [0  ,140,255;
             0  ,140,255;
             0  ,140,255];       
end

if(cas == 2)
    patch = [255,255,255;
             140,140,255;
             0  ,140,255];       
end

if(cas == 3)
    patch = [255,255,255;
             255,140,140;
             255,140,0  ];       
end
if(cas==4)
   patch = [255,140,0;
            255,140,140;
            255,255,255];       
end
if(cas==5)
    5
    patch = [0, 140,255;
            140,140,255;
            255,255,255];      
end





end
