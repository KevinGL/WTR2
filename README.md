# WTR2
Cette mini-lib codée en C++ permet de simuler des effets météo et de représenter le ciel en OpenGL 3.x

Il faut déclarer un objet WTR2_Sky et l'initialisation avec la méthode Init() dont voici les paramètres :

- Le rapport de vitesse de déplacement du Soleil
- L'heure initiale
- Le jour (Du mois)
- Le mois (En chiffre)
- La latitude sur la planète
- Le chemin d'accès vers un fichier à créer avec mon générateur d'étoiles
- La couleur de l'horizon (Structure WTR2_Color)
- La couleur du zenith
- La couleur du Soleil
- La couleur du Soleil au coucher
- Les dimensions de votre fenêtre

Pour le rendu il faut appeler la méthode Draw() avec pour paramètres :

- La position de la caméra
- La matrice de projection
- La matrice modèle-vue

A partir de là le rendu du Soleil et des étoiles se fait automatiquement
