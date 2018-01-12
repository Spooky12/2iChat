#include "../libs/include.h"

/***
 * @name ping
 * @brief Fonction permettant de répondre pong à un ping et ping à un pong
 * @param message
 * @param ping
 */
void ping(char *message, int ping){
	if(ping){
		sprintf(message, "201\npong\n");//Si ping répondre pong
	}else{
		sprintf(message, "201\nping\n");//Sinon ping
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
	sprintf(message, "201\n");//On prépare une réponse à une commande
	for(c=salon->clients; c != NULL; c=c->hh.next) {//On parcourt la liste des clients dans le salon
		sprintf(message,"%s\n%s",message, c->nom);//On ajoute le client à la réponse
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
	sprintf(message, "201\n");//On prépare une réponse à une commande
	for(s=salon; s != NULL; s=s->hh.next) {//On parcourt la liste des serveurs existants
		sprintf(message,"%s\n%s",message, s->nom);//On ajoute le serveur à la réponse
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
	char ps[BUFF_MAX];//nouveau pseudo
	char ancien[BUFF_MAX];//ancien pseudo
	split(ps,texte);//on récupère le pseudo dans le texte envoyé
	if(strlen(ps)<10 && strlen(ps)>0){//Si le pseudo n'est pas nul et que sa taille est inférieure à 10
		HASH_FIND_STR( salon->clients, ps, c);//On cherche si ce pseudo existe déja dans le serveur
		if(c == NULL){//Si ce pseudo n'existe pas déja
			strcpy(ancien, client->nom);//On sav l'ancien pseudo
			strcpy(client->nom,ps);//On modifie le pseudo de l'utilisateur
			sprintf(message, "201\n%s s'appelle maintenant %s\n", ancien, client->nom);//On envoie une info à tout le monde
			HASH_DEL( salon->clients, client );//On supprime le client du salon
			HASH_ADD_STR( salon->clients, nom, client );//On ajoute le client au salon avec son nouveau pseudo
			return 1;
		}else{//Sinon on retourne une erreur
			sprintf(message, "400\nUn client possède déja ce pseudo dans ce salon\n");
			return 0;
		}
	}else{//Sinon on retourne une erreur
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
	char cou[BUFF_MAX];//couleur textuelle
	int couleur;//couleur chiffrée

	split(cou,texte);//on récupère la couleur dans le texte envoyé
	if (strlen(cou) == 0){
		sprintf(message, "201\nPour changer de couleur tapez \\color code avec code entre 1 et 8\n");
	}else{
		couleur = atoi(cou);//On convertit le texte en int
		if(couleur < 9 && couleur > 0){//Si la couleur est entre 1 et 9 on la modifie
			client->couleur = couleur-1;
			sprintf(message, "201\nCouleur bien changée\n");
		}else{//Sinon, on renvoie une erreur
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
	sprintf(message, "211\n%s", client->nom);//On prépare une réponse à une commande
	split(temp, param);//on récupère le premier mot à transmettre
	while(strcmp(param, "")!=0){//tant qu'il existe des mots à transmettre
		sprintf(message, "%s %s", message, temp);//On ajoute le mot à la phrase
		split(temp, param);//On récupère le mot suivant
	}
	strcat(message,"\n");//On ajoute un séparateur à la fin
}

/***
 * @name info
 * @brief Fonction permettant d'obtenir des informations sur le salon actuel
 * @param message
 * @param client
 * @param salon
 */
void info(char *message, struct Client *client, struct Salon *salon){
	sprintf(message, "201\nBonjour %s", client->nom);//On affiche le nom du client
	sprintf(message, "%s\nVous êtes sur le salon %s", message, salon->nom);//Puis le nom du serveur
	int nb = HASH_COUNT(salon->clients);//On récupère le nombre de clients connectés sur le serveur
	if(nb==1){//On affiche le singulier ou le pluriel
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
void alea(struct Client *client, char *message, char *param){
	char tmp[BUFF_MAX];
	split(tmp, param);//On récupère le premier element
	int a = atoi(tmp);//On le convertis en int
	split(tmp, param);//On récupère le second element
	int b = atoi(tmp);//On le convertis en int
	sprintf(message, "201\%s à tiré un nombre aléatoire entre %d et %d et à obtenu %d\n",client->nom, min(a,b), max(a,b), (rand()% (max(a,b) - min(a,b) + 1)) + min(a,b));//on calcule un nombre aléatoire entre nos deux valeurs et on le renvoie
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
	
	if(strcmp((*salon)->nom, "Accueil")==0){//On ne peux créer de salon que depuis l'accueil
		split(nom, param);//on récupère le nom du salon
		if(strlen(nom)<64 & strlen(nom)>0){//Si la longueur du nom est inférieure à 64 et non nulle
			HASH_FIND_STR( (*salon), nom, newSalon);//On cherche un salon ayant ce nom
			if(newSalon == NULL){//Si il n'existe pas de salon ayant ce nom
				newSalon = (struct Salon*)malloc(sizeof(struct Salon));//On crée un nouveau salon
				//On lui attribue son nom, mot de passe et liste de clients
				strcpy(newSalon->nom,nom);
				strcpy(newSalon->mdp,"");
				newSalon->clients = NULL;
				
				HASH_ADD_STR( (*salon), nom, newSalon);//On ajoute le salon à la liste des salons existants
				HASH_DEL( (*salon)->clients, client );//On supprime le client de l'ancien salon
				*salon = newSalon;//On modifie le salon du client
				HASH_ADD_STR( (*salon)->clients, nom, client );//On ajoute le client au nouveau salon
				sprintf(message, "201\nSalon %s bien créé\n", newSalon->nom);//On envoie une confirmation au client
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
	struct Client *c;
	char nom[BUFF_MAX];
	
	if(strcmp((*salon)->nom, "Accueil")==0){//Il n'est possiblle de changer de salon que depuis l'accueil
		split(nom, param);//on récupère le nom du salon
		HASH_FIND_STR( (*salon), nom, s);//On cherche un salon ayant ce nom
		if(s != NULL){//Si ce salon existe
			HASH_FIND_STR(s->clients , client->nom , c);//On cherche un salon ayant ce nom
			if(c == NULL){//Si il n'existe pas de client avec le même pseudo dans le salon à rejoindre
				HASH_DEL( (*salon)->clients, client );//On supprime le client de l'ancien salon
				*salon = s;//On modifie le salon du client
				HASH_ADD_STR( (*salon)->clients, nom, client );//On ajoute le client au nouveau salon
				sprintf(message, "201\nVous avez rejoint le salon %s\n", nom);//On envoie une confirmation au client
			}else{
				sprintf(message, "400\nUn client ayant le même pseudo que vous est déja dans le salon %s. Veuillez changer de pseudo pour rejoindre ce salon",nom);
			}
		}else{
			sprintf(message, "400\nLe salon %s n'existe pas\nUtilisez \\create %s pour créer ce salon",nom,nom);
		}
	}else{
		sprintf(message, "400\nOn ne peut rejoindre un salon que depuis l'accueil\n");
	}
}