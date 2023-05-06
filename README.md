
#Pour compiler:
#Version opengl 3.3 pour GLAD
g++ src/main.cpp src/glad.c -I./include/ -L./lib/ -o bin/projet -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
#Pour les pc de la fac enlever -lXrandr -lXi -ldl

#Pour exécuter:
./bin/projet

#Les commandes:
#Pour quitter utiliser la touche [échap]
#Pour se déplacer utiliser [Z][Q][S][D]
#Pour s'orienter utiliser la souris

#Remarque : la 1ère lumière est située au même endroit
#que la courbe de Bézier, donc il faut se déplacer derrière les différentes surfaces 
#pour voir l'effet des microfacettes
#Les lumières 2 et 3 sont sur la surface avec l'éclairage lambertien simple
# et la surface avec les normales interpolées
#et les surfaces microfacettes avec ou sans texture et avec ou sans les 3 lumières sont vertes
#cf: les variables lightPos et multipleLightPos dans main.cpp
