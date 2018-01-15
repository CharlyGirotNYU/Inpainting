function [ patch ] = invers_patch( old_patch )

for i=1:3
    for j=1:3
        
        if(old_patch (i,j) == 0)
            patch(i,j) = 140;
        end
        if(old_patch (i,j) ==140)
            patch(i,j) = 0;
        end
        if(old_patch (i,j) == 255)
            patch(i,j) = 255;
        end
    
end


end

