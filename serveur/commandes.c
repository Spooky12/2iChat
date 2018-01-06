#include "../libs/include.h"

/***
 * @name ping
 * @brief Fonction permettant de répondre pong à un ping et ping à un pong
 * @param message
 * @param ping
 */
void ping(char *message, int ping){
	if(ping){
		sprintf(message, "201\npong\n");
	}else{
		sprintf(message, "201\nping\n");
	}
}

/***
 * @name listeClients
 * @brief Fonction permettant de recupérer la liste des clients d'un salon
 * @param message
 * @param salon
 */
void listeClients(char *message, struct Salon *salon){
	struct Client *c;
	sprintf(message, "201\n");
	for(c=salon->clients; c != NULL; c=c->hh.next) {
		strcat(message, c->nom);
		strcat(message,"\n");
    }
}

/***
 * @name listeSalons
 * @brief Fonction permettant de recupérer la liste des salons existant
 * @param message
 * @param salon
 */
void listeSalons(char *message, struct Salon *salon){
	struct Salon *s;
	sprintf(message, "201\n");
	for(s=salon; s != NULL; s=s->hh.next) {
		strcat(message, s->nom);
		strcat(message,"\n");
    }
}

/***
 * @name pseudo
 * @brief Fonction permettant de changer de pseudo
 * @param message
 * @param client
 * @param salon
 * @param texte
 */
int pseudo(char *message, struct Client *client, struct Salon *salon, char *texte){
	struct Client *c;
	char ps[BUFF_MAX];
	char ancien[BUFF_MAX];
	split(ps,texte);
	if(strlen(ps)<10 && strlen(ps)>0){
		HASH_FIND_STR( salon->clients, ps, c);
		if(c == NULL){
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

/***
 * @name couleur
 * @brief Fonction permettant de changer de couleur
 * @param message
 * @param client
 * @param texte
 */
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

/***
 * @name me
 * @brief Fonction permettant de formater une commande de type \me
 * @param message
 * @param client
 * @param param
 */
void me(char *message, struct Client *client, char *param){
	char temp[BUFF_MAX] = "";
	sprintf(message, "211\n%s", client->nom);
	split(temp, param);
	while(strcmp(param, "")!=0){
		sprintf(message, "%s %s", message, temp);
		split(temp, param);
	}
	strcat(message,"\n");
}

/***
 * @name info
 * @brief Fonction permettant d'obtenir des informations sur le salon actuel
 * @param message
 * @param client
 * @param salon
 */
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

/***
 * @name min
 * @brief Fonction permettant d'obtenir le min entre a et b
 * @param a
 * @param b
 */
int min(int a, int b){
	if(a<b){
		return a;
	}
	return b;
}

/***
 * @name max
 * @brief Fonction permettant d'obtenir le max entre a et b
 * @param a
 * @param b
 */
int max(int a, int b){
	if(a>b){
		return a;
	}
	return b;
}

/***
 * @name alea
 * @brief Fonction permettant d'obtenir un nombre aléatoire entre deux nombres donnés
 * @param message
 * @param param
 */
void alea(char *message, char *param){
	char tmp[BUFF_MAX];
	split(tmp, param);
	int a = atoi(tmp);
	split(tmp, param);
	int b = atoi(tmp);
	sprintf(message, "201\nNombre aléatoire: %d\n", (rand()% (max(a,b) - min(a,b) + 1)) + min(a,b));
}

/***
 * @name creation
 * @brief Fonction permettant de créer un salon
 * @param message
 * @param param
 * @param salon
 * @param client
 */
void creation(char *message, char *param, struct Salon **salon, struct Client *client){
	struct Salon *newSalon;
	char nom[BUFF_MAX];
	if(strcmp((*salon)->nom, "Accueil")==0){	
		split(nom, param);
		if(strlen(nom)<64 & strlen(nom)>0){
			HASH_FIND_STR( (*salon), nom, newSalon);
			if(newSalon == NULL){
				newSalon = (struct Salon*)malloc(sizeof(struct Salon));
				strcpy(newSalon->nom,nom);
				strcpy(newSalon->mdp,"");
				newSalon->clients = NULL;
				HASH_ADD_STR( (*salon), nom, newSalon);
				HASH_DEL( (*salon)->clients, client );
				*salon = newSalon;
				HASH_ADD_STR( (*salon)->clients, nom, client );
				sprintf(message, "201\nSalon %s bien créé\n", newSalon->nom);
			}else{
				sprintf(message, "400\nUn salon de ce nom existe déja\n");
			}
		}else{
			sprintf(message, "400\nLe nom du salon doit être compris entre 1 et 64 caracteres\n");
		}
	}else{
		sprintf(message, "400\nOn ne peut créer de salon que dans l'accueil\n");
	}
}

/***
 * @name connection
 * @brief Fonction permettant de rejoindre un salon
 * @param message
 * @param param
 * @param salon
 * @param client
 */
void connection(char *message, char *param, struct Salon **salon, struct Client *client){
	struct Salon *s;
	char nom[BUFF_MAX];
	if(strcmp((*salon)->nom, "Accueil")==0){	
		split(nom, param);
		HASH_FIND_STR( (*salon), nom, s);
		if(s != NULL){
			HASH_DEL( (*salon)->clients, client );
			*salon = s;
			HASH_ADD_STR( (*salon)->clients, nom, client );
			sprintf(message, "201\nVous avez rejoint le salon %s\n", nom);
		}else{
			sprintf(message, "400\nLe salon %s n'existe pas\nUtilisez \\create %s pour créer ce salon",nom,nom);
		}
	}else{
		sprintf(message, "400\nOn ne peut rejoindre un salon que depuis l'accueil\n");
	}
}