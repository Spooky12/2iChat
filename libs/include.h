// Includes standards
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ncurses.h>

// Includes perso
#include "uthash.h" //Gestion des hashsets

// Macros
#define CHECK(v,mess) if (v<0){perror(mess);	exit(0);}

// Constantes
#define PORT 5000
#define BUFF_MAX 1024

// Structures
struct Client {
    int socket;
    char nom[10];
	int couleur;
    UT_hash_handle hh;
};

struct Salon {
	char nom[64];
	char mdp[64];
	struct my_struct *clients;
	UT_hash_handle hh;
};

// affichage.c
void initialiserCurses();
void initialiserCouleurs();
void afficherLigne(WINDOW *win, char* texte);
void afficherMessage(WINDOW *win,char* pseudo, char* texte, int couleur);
void fermerCurses();

// client.c
void lire_reponse(int soc);
void envoyer_requete(int soc, char *req);

// serveur.c
int lire_requete(int soc, char * param);
void traiter_req0();
void traiter_req100();