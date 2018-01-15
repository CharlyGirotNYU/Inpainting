
clear all; close all;
grid on; hold on;

% Vert = Mask
% Jaune = Border
% Bleu = Source

patch = create_patch(4);

i_patch = invers_patch(patch);

% patch = flipud(patch);
% imagesc(patch);
colormap gray;



%% Ecrire ici retourner par l'affichage Qt
n_p = [0,0]
n_p=[n_p;-0.707,0.707];
n_p(2,2) = -n_p(2,2);

plot(n_p(:,1),n_p(:,2));
axis([-1,1 ,-1,1]);
% normal = [2,2;n_p(1)+2,-(n_p(2)+2)];
% 
% droite =correct_d(d);
% hold on; 
% plot(normal(:,1),normal(:,1),'r-*');
% plot(droite(:,1),droite(:,2),'b-*');
