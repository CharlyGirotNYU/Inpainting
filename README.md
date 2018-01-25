# Inpainting
InPainting - Projet Majeur CPE 2018 - 5 ETI IMI

DI FOLCO Maxime - GIROT Charly - JALLAIS Maëliss

L'archive fournie est composée des dossiers suivants : 
	- images : Contient les images d'entrées, les masques, et les résultats après application de l'algorithme
	- opencv_project : Code pour tester la fonction haut niveau d'inpainting de OpenCV et pouvoir comparer les résultats avec notre implémentation
	- Project : Contient le code source du projet
	- resume : Contient les résumés des publications composant notre bibliographie
	- Zotero : Bibliographie
	- une licence, l'énoncé du projet et le poster de présentation de notre projet


Pour compiler le projet : 
	- cd Project
	- mkdir build
	- cd build
	- cmake ..
	- make
	- ./inpainting


Projet/src/main.cpp : 
	- Pour changer l'image d'entrée et le masque, faire image.imread(input, mask). Le masque doit être rouge au niveau du masque et noir partout ailleurs.
	- Possibilité de changer la taille du patch lorsque l'on créé la région : RegionFill region = RegionFill(taille_x, taille_y)
	- Possibilité de changer le nom et l'emplacement de l'image résultat avec la commande image.imwrite("emplacement.png")
