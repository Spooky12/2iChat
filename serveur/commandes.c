#include "../libs/include.h"

void ping(char *message, int ping){
	if(ping){
		sprintf(message, "201\npong");
	}else{
		sprintf(message, "201\nping");
	}
}

void listeClients(char *message, struct Salon *salon){
	struct Client *c;
	sprintf(message, "201\n");
	for(c=salon->clients; c != NULL; c=c->hh.next) {
		strcat(message, c->nom);
		strcat(message,"\n");
    }
}

void listeSalons(char *message, struct Salon *salon){
	struct Salon *s;
	sprintf(message, "201\n");
	for(s=salon; s != NULL; s=s->hh.next) {
		strcat(message, s->nom);
		strcat(message,"\n");
    }
}

int pseudo(char *message, struct Client *client, struct Salon *salon, char *texte){
	struct Client *c;
	char ps[BUFF_MAX];
	char ancien[BUFF_MAX];
	split(ps,texte);
	if(strlen(ps)<10 && strlen(ps)>0){
		HASH_FIND_STR( salon->clients, message, c);
		if((strcmp(salon->nom,"Accueil")==0) || c == NULL){
			strcpy(ancien, client->nom);
			strcpy(client->nom,ps);
			sprintf(message, "201\n%s s'appelle maintenant %s", ancien, client->nom);
			return 1;
		}else{
			sprintf(message, "400\nUn client possède déja ce pseudo dans ce salon");
			return 0;
		}
	}else{
		sprintf(message, "400\nErreur lors du changement de pseudo");
		return 0;
	}
}

void couleur(char *message, struct Client *client, char *texte){
	struct Client *c;
	char cou[BUFF_MAX];
	int couleur;

	split(cou,texte);
	if (strlen(cou) == 0){
		//TODO expliquer le fonctionnement de couleur
	}else{
		couleur = atoi(cou);
		if(couleur < 8 && couleur >= 0){
			client->couleur = couleur;
			sprintf(message, "201\nCouleur bien changée");
		}else{
			sprintf(message, "400\nErreur lors du changement de couleur");
		}
	}
}

void me(){
	
}

void info(){
	
}

void alea(){
	
}

void connection(){
	
}