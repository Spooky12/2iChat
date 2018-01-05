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

void pseudo(char *message, struct Client *client, struct Salon *salon, char *texte){
	struct Client *c;
	char ps[BUFF_MAX];
	//On recupere la première occurence de \n
	char *d = strstr(texte, "\n");
	if(d==NULL){
		strcpy(ps, texte);
	}else{
		//On calcule la position du premier \n
		int posi = d - texte;
		//On recupere le pseudo
		strncpy ( ps, texte, posi );
	}
	if(strlen(ps)<10 && strlen(ps)>0){
		HASH_FIND_STR( salon->clients, message, c);
		if((strcmp(salon->nom,"Accueil")==0) || c== NULL){
			strcpy(client->nom,ps);
			strcat(message, "201\nPseudo bien changé");
			strcat(message,"\n");
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