# Chat2i

Chat2i est une chat multi-client/serveur developpé dans le cadre des cours de MCS

#### Compilation
##### Compilation du serveur
```
gcc commandes.c -c
gcc commandes.o serveur.c -lpthread -o serveur
```
##### Compilation du client
```
gcc -c affichage.c
gcc affichage.o client.c -lpthread -lncurses -o client
```

#### Commandes
Des commandes spécifiques peuvent être utilisées pour certaines tâches. Elles commencent toutes par le caractère `\` et peuvent nécessiter un paramètre.
Les paramètres requis sont désignés par `<` et `>`.
Les paramètres optionels sont désignés par `[` et `]`.
##### Liste des commandes 
* `\connect <nomSalon>` Depuis l'accueil, permet de se connecter au salon `nomSalon`
* `\leave` Permet de quitter le salon dans lequel on se trouve (Hors acceuil)
* `\create <nomSalon>` Depuis l'accueil, permet de créer un nouveau salon `nomSalon`
* `\exit` Permet de fermer le programme
* `\pseudo <pseudo>` Permet de changer de pseudo
* `\couleur <nombre>` Permet de changer la couleur de ses messages, `nombre` doit être compris entre 1 et 7
* `\info` Donne des informations sur le salon dans lequel l'utilisateur se trouve
* `\list` Dans l'accueil, affiche la liste des salons. Dans un salon, affiche la liste des utilisateurs conencté sur le salon
* `\ping` Le serveur répond pong 
* `\pong` Le serveur  
* `\rand <nombre1> [nombre2]` Le serveur  répond avec un nombre aléatoire entre `nombre1` et `nombre2` (entre 0 et `nombre1` si `nombre2` n'est pas renseigné)
* `\me <message>` Le pseudo s'affiche suivi de `message` 
