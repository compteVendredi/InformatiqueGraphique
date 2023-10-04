
Projet de visualiastion de courbes et de surfaces de Bézier selon différents éclairages (lambertien simple, modèle de réflectance à microfacette, normale interpolée) avec OpenGL.


# Pour compiler:
Il faut la version opengl 3.3 pour GLAD et faire :
```
g++ src/main.cpp src/glad.c -I./include/ -L./lib/ -o bin/projet -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl
```

# Pour exécuter:
```
./bin/projet
```

# Les commandes:
Pour quitter utiliser la touche [échap]

Pour se déplacer utiliser [Z][Q][S][D]

Pour s'orienter utiliser la souris

Remarques : 
- Il y a 3 lumières
- La 1ère lumière est située au même endroit que la courbe de Bézier, donc il faut se déplacer derrière les différentes surfaces pour voir l'effet des microfacettes
- Les lumières 2 et 3 sont sur la surface avec l'éclairage lambertien simple et la surface avec les normales interpolées et les surfaces microfacettes avec ou sans texture et avec ou sans les 3 lumières sont vertes
