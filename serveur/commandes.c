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

void pseudo(){
	
}

void me(){
	
}

void info(){
	
}

void alea(){
	
}

void connection(){
	
}