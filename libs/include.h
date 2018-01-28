// Includes standards
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <ncurses.h>
#include <pthread.h>

// Includes perso
#include "uthash.h" //Gestion des hashsets

// Macros
#define CHECK(v,mess) if (v<0){perror(mess);	fermerCurses(); exit(0);}

// Constantes
#define PORT 5000
#define PORT_PM 5001
#define BUFF_MAX 1024

// Structures
struct Client {
    int socket;
    char nom[10];
	char ip[16];
	int couleur;
    UT_hash_handle hh;
};

struct Salon {
	char nom[64];
	char mdp[64];
	struct Client *clients;
	UT_hash_handle hh;
};

// affichage.c
void initialiserCurses();
void reloadTerminal();
void initialiserCouleurs();
void drawChatWin();
void drawInputWin();
void splashScreen();
void changerTitre(char *nouveauTitre);
void sortieSalon(WINDOW *win);
void entreeSalon(WINDOW *win, char *nomSalon, int creationSalon);
void afficherLigne(WINDOW *win, char* texte);
void afficherMessage(WINDOW *win,char* pseudo, char* texte, int couleur);
void recupererMessage(char *message);
void fermerCurses();
void afficherMessageServeur(WINDOW *win, char *text);
void afficherErreur(WINDOW *win, char* erreur, char *text);
void help();

// client.c
void deroute(int signal_number);
void *gestion_envoie(void *soc);
void *gestion_lecture(void *soc);
void split(char *dest, char *params);
void lire_reponse(int soc);
void demandePrive(char *params);
void startPrive(int soc, char *req);
void connecterPrive(char *params);
void envoyer_message(int soc, char *req, char *message);
void envoyer_messagePrive(char *req, char *message);
void envoyer_commande(int soc, char *req, char *commande);
void envoyer_requete(int soc, char *req);

// serveur.c
int lire_requete(int soc, char * param);
void envoyer_reponse(int soc, char *req);
void diffusion(struct Salon *salon, char* message);
void traiter_req0(int soc);
void traiter_req100(struct Salon *salon,struct Client *client, char* texte);
void traiter_req101(struct Salon **salon,struct Client *client, char* texte);
void traiter_req300(struct Salon *salon,struct Client *client, int resultat, char* texte);
void* traiterClient(void* ptr);
void dem_lecture();
void dem_ecriture();
void rendre();

// commandes.c
void ping(char * message, int ping);
void listeClients(char *message, struct Salon *salon);
void listeSalons(char *message, struct Salon *salon);
int pseudo(char *message, struct Client *client, struct Salon *salon, char *texte);
void couleur(char *message, struct Client *client, char *texte);
void me(char *message, struct Client *client, char *texte);
void info(char *message, struct Client *client, struct Salon *salon);
void alea(struct Client *client, char *message, char* param);
void creation(char *message, char *param, struct Salon **salon, struct Client *client);
void connection(char *message, char *param, struct Salon **salon, struct Client *client);
void leave(char *message, struct Salon **salon, struct Client *client);
void prive(char *message,char *param, struct Client *client, struct Salon *salon);