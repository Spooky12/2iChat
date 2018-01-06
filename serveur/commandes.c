#include "../libs/include.h"

void ping(char *message, int ping){
	if(ping){
		sprintf(message, "201\npong\n");
	}else{
		sprintf(message, "201\nping\n");
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
			sprintf(message, "201\n%s s'appelle maintenant %s\n", ancien, client->nom);
			return 1;
		}else{
			sprintf(message, "400\nUn client possède déja ce pseudo dans ce salon\n");
			return 0;
		}
	}else{
		sprintf(message, "400\nErreur lors du changement de pseudo\n");
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
		if(couleur < 9 && couleur > 0){
			client->couleur = couleur-1;
			sprintf(message, "201\nCouleur bien changée\n");
		}else{
			sprintf(message, "400\nErreur lors du changement de couleur\n");
		}
	}
}

void me(char *message, struct Client *client, char *param){
	char temp[BUFF_MAX] = "";
	sprintf(message, "211\n%s", client->nom);
	split(temp, param);
	while(strcmp(param, "")!=0){
		sprintf(message, "%s %s", message, temp);
		split(temp, param);
	}
}

void info(char *message, struct Client *client, struct Salon *salon){
	sprintf(message, "201\nBonjour %s", client->nom);
	sprintf(message, "%s\nVous êtes sur le salon %s", message, salon->nom);
	int nb = HASH_COUNT(salon->clients);
	if(nb==1){
		sprintf(message, "%s\n%d personne est actuellement sur ce salon", message, nb);
	}else{	
		sprintf(message, "%s\n%d personnes sont actuellement sur ce salon", message, nb);
	}
}

void alea(){
	
}

void connection(){
	
}